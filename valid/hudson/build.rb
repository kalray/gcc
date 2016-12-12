#!/usr/bin/ruby
#
# Copyright (C) 2008-2016 Kalray SA.
#
# All rights reserved.
#

$LOAD_PATH.push('metabuild/lib')
require 'metabuild'
require 'copyrightCheck'
require 'qualityChecker'
include Metabuild

options = Options.new({ "target"        => ["k1", "Target arch"],
                        "clone"         => [".", "Path to the clone"],
                        "build_type"    => ["Debug", "Can be Release or Debug." ], 
                        "platform"      => ["linux", "Platforms are: 'linux', 'mingw32'."],
                        "version"       => ["unknown", "Version of the delivered compiler."],
                        "processor"     => {"type" => "string", "default" => "processor", "help" => "processor clone relative to workspace path" },
                        "mds"           => {"type" => "string", "default" => "mds", "help" => "mds clone relative to workspace path" },
                        "bootstrap"     => {"type" => "boolean", "default" => false, "help" => "Bootstrap phase of gcc build."},
                        "variant"       => {"type" => "keywords", "keywords" => [:nodeos, :elf, :rtems, :linux], "default" => "elf", "help" => "Select build variant."},
                        "sysroot"       => ["sysroot", "Sysroot directory"],
                        "k1Req"         => ["kEnv/k1tools/usr/local/k1Req", "Path to k1Req env containing gmp, mpfr and mpc."],
                        "core"          => ["k1io", "The core to build for (only for Linux builds)"],
                        "march_valid"   => ["k1b:k1bio,k1bdp", "List of mppa_architectures to validate on execution_platform."],
                      })

workspace  = options["workspace"]
gcc_clone  = options["clone"]
gcc_path   = File.join(workspace,options["clone"])
core       = options["core"]
march_valid_hash = Hash[*options["march_valid"].split(/::/).map{|tmp_arch| tmp_arch.split(/:/)}.flatten]

march_valid_list    = march_valid_hash.keys

k1Req       = File.expand_path(options["k1Req"])
gmp_prefix  = File.join(k1Req,"gmp")
mpfr_prefix = File.join(k1Req,"mpfr")
mpc_prefix  = File.join(k1Req,"mpc")
configure_with="--with-gmp=#{gmp_prefix} --with-mpfr=#{mpfr_prefix} --with-mpc=#{mpc_prefix} --with-gnuas --with-gnu-as --with-gnu-ld"

repo = Git.new(gcc_clone,workspace)

arch      = options["target"]
$arch     = options["target"]
pkg_prefix_name = options.fetch("pi-prefix-name","#{$arch}-")

variant   = options["variant"].to_s
target_variant = "_" + variant
variant_dir= "#{arch}-#{variant}"
bootstrap = options["bootstrap"]
s_bootstrap = bootstrap ? "_bootstrap" : ""


build_type= options['build_type']
platform  = options['platform']

clean = CleanTarget.new("clean" + target_variant, repo, [])

common_pre_build = ParallelTarget.new("common_pre_build" + s_bootstrap + target_variant, repo, [])
common_pre_build.write_prefix()

build = ParallelTarget.new("build" + s_bootstrap + target_variant, repo, [common_pre_build], [])
# build write in prefix all sys-include files when --with-headers is set.
# It fails also when building for Linux at bootstrap
build.write_prefix()

valid = ParallelTarget.new("valid" + s_bootstrap + target_variant, repo, [build], [])

install = ParallelTarget.new("install" + s_bootstrap + target_variant, repo, [valid], [])
install.write_prefix()

package = Target.new("package" + s_bootstrap + target_variant, repo, [install], [])
check = Target.new("check", repo, [], [])

libgomp = ParallelTarget.new("libgomp", repo, [])
valid_valid = ParallelTarget.new("gcc", repo, [])
libgomp_valid = ParallelTarget.new("libgomp_valid", repo, [build])

copyright_check = Target.new("copyright_check", repo, [], [])

b = Builder.new("gcc", options, [clean, build, valid, install, package, check, valid_valid, libgomp_valid, libgomp, common_pre_build, copyright_check])

version   = options['version']

build_dirs    = []
flag_options  = []
lib_options   = []

debug_flags = "CFLAGS=\"-O0 -g3\" CXXFLAGS=\"-O0 -g3\""

b.builder_infos.each do |builder_info|
  # Only native build is needed for gcc
  if(builder_info.native) then
    bootstrap_str = bootstrap ? "_bootstrap" : ""
    build_dirs.push  File.join(gcc_path,"#{arch}_#{variant}#{bootstrap_str}_#{build_type}_build_#{builder_info.type}")
    flag_options.push builder_info.cflags
    lib_options.push builder_info.lib
  end
end

b.default_targets = [package]

b.logsession = arch

prefix          = options.fetch("prefix", File.expand_path("none",workspace))
install_prefix  = File.join(prefix,"gcc","devimage","gcc-#{variant}")
install_libstdcpp_prefix  = File.join(prefix,"gcc","devimage","libstdc++-#{variant}")
install_libgomp_prefix    = File.join(prefix,"gcc","devimage","libgomp-#{variant}")
install_info_prefix    = File.join(prefix,"gcc","devimage","info-#{variant}")
install_libgfortran_prefix  = File.join(prefix,"gcc","devimage","libgfortran-#{variant}")
toolroot        = options.fetch("toolroot", prefix)
sysroot = options["sysroot"]

sysroot_option  = ""

install_paths = [ toolroot, install_prefix ]
install_paths = [ toolroot ] if(bootstrap)

sysroot_sd = ""
if(variant == "linux") then
  # Get subdirectory relative to toolroot of sysroot.
  sysroot_sd = b.diffdirs(sysroot, toolroot)

  install_paths = [ sysroot, install_prefix ]
  install_paths = [ sysroot ] if(bootstrap)
end

env = { "PATH" => File.join(toolroot,"bin") + ":" + ENV.fetch("PATH","") }

kalray_internal = File.join(toolroot,"kalray_internal")

b.logtitle = "Report for gcc, arch = #{arch}"

processor_clone = options["processor"]
processor_path = File.join(workspace,processor_clone)

case arch
  when "k1" then 
  family_prefix = "#{processor_path}/#{arch}-family"
  else raise "Unknown target: #{arch}"
end


b.target("check") do
	cd gcc_path
	build_path = File.join(gcc_path, "codechecks")
	mkdir_p build_path

	qc = QualityChecker.new(b, k1Req)
	qc.code_checks(["gcc/config/k1/k1.c"], build_path + "/code_checks.perf", "-D" , "")
	b.report_perf_files("gcc", [build_path + "/code_checks.perf"])

	rm_rf build_path
end


b.target("common_pre_build" + s_bootstrap + target_variant) do
  
  puts "========= GCC Pre-Building for Linux in #{build_type} mode. =========\n"
  version = options["version"] + " " + `git rev-parse --verify --short HEAD 2> /dev/null`.chomp
  version += "-dirty" if not `git diff-index --name-only HEAD 2> /dev/null`.chomp.empty?
  
  b.run("echo \"[Kalray Compiler #{version}]\" > #{File.join(gcc_path,"gcc","REVISION")}")
  
  install_paths.each do |install_path|
    mkdir_p File.join(install_path,"bin")
    b.run("cp -f #{gcc_path}/valid/hudson/#{arch}-gcc.sh       #{install_path}/bin/#{arch}-gcc")
    b.run("cp -f #{gcc_path}/valid/hudson/#{arch}-gcov.sh      #{install_path}/bin/#{arch}-gcov")
    b.run("cp -f #{gcc_path}/valid/hudson/#{arch}-g++.sh       #{install_path}/bin/#{arch}-g++")
    b.run("cp -f #{gcc_path}/valid/hudson/#{arch}-g++.sh       #{install_path}/bin/#{arch}-c++")
    b.run("cp -f #{gcc_path}/valid/hudson/#{arch}-gfortran.sh  #{install_path}/bin/#{arch}-gfortran")
  end
end


b.target("build" + s_bootstrap + target_variant) do

  build_dirs.zip(lib_options, flag_options).each do |build_dir, lib_opt, flag_opt|
    puts "========= Building for Linux in #{build_type} mode. =========\n"
    
    gmp_lib=File.join(gmp_prefix,lib_opt)
    mpfr_lib=File.join(mpfr_prefix,lib_opt)
    mpc_lib=File.join(mpc_prefix,lib_opt)

    with_extra_flags="--with-mpc-lib=#{mpc_prefix}/#{lib_opt} --with-mpfr-lib=#{mpfr_prefix}/#{lib_opt} --with-gmp-lib=#{gmp_prefix}/#{lib_opt}"

    b.create_goto_dir! build_dir

    case arch
    when "k1" then 
      prog_prefix = "k1-" + variant + '-'
      target = "k1-#{variant}"
    else raise "Unknown target: #{arch}"
    end

    gomp = "--disable-libgomp"
    gomp = "--enable-libgomp" if(variant == "nodeos")

    if variant == "linux" then
      newlib = ""
      libatomic_toggle = "--disable-libatomic"
      threads = "--disable-threads"
      quadmath = "--disable-libquadmath"
      languages = "c,c++,fortran"
      sysroot_option = "--with-sysroot=#{sysroot} --with-build-sysroot=#{sysroot}"
      include_path = "include"
      multilib = "--enable-multilib"
    elsif variant == "nodeos"       
      newlib = "--with-newlib"
      libatomic_toggle = ""
      threads = "--enable-threads=posix"
      quadmath = ""
      languages = "c,c++,fortran"
#      sysroot_option = "--with-build-time-tools=#{toolroot}/#{variant_dir}/bin --with-sysroot=#{toolroot} --with-native-system-header-dir=/#{variant_dir}/include"
      sysroot_option = "--with-build-time-tools=#{toolroot}/#{variant_dir}/bin" # --with-headers=#{toolroot}/#{variant_dir}/include"
      multilib = "--enable-multilib"
      include_path = "sys-include"
    elsif variant == "rtems"
      newlib = "--with-newlib"
      threads = "--enable-threads"
      libatomic_toggle = "--disable-libatomic"
      quadmath = ""
      languages = "c,c++,fortran"
#      sysroot_option = "--with-build-time-tools=#{toolroot}/#{variant_dir}/bin --with-sysroot=#{toolroot} --with-native-system-header-dir=/#{variant_dir}/include"
      sysroot_option = "--with-build-time-tools=#{toolroot}/#{variant_dir}/bin" # --with-headers=#{toolroot}/#{variant_dir}/include"
      multilib = "--enable-multilib"
      include_path = "sys-include"
    else
      newlib = "--with-newlib"
      threads = "--enable-threads"
      libatomic_toggle = ""
      quadmath = ""
      languages = "c,c++,fortran"
#      sysroot_option = "--with-build-time-tools=#{toolroot}/#{variant_dir}/bin --with-sysroot=#{toolroot} --with-native-system-header-dir=/#{variant_dir}/include"
      sysroot_option = "--with-build-time-tools=#{toolroot}/#{variant_dir}/bin" # --with-headers=#{toolroot}/#{variant_dir}/include"
      multilib = "--enable-multilib"
      include_path = "sys-include"
    end

    configure_common = "#{with_extra_flags} " +
                       "--prefix=#{toolroot} " +
                       "#{libatomic_toggle} " +
                       "#{configure_with} " +
                       "--disable-bootstrap " +
                       "--disable-shared " +
                       "#{multilib} " +
                       "--disable-libmudflap " +
                       "--disable-libssp " +
                       "#{quadmath} " +
                       "--enable-checking=release "+
                       "--with-system-zlib " +
                       "--enable-__cxa_atexit " +
                       "--target=#{variant_dir} " +
                       "--program-prefix=#{prog_prefix} " +
                       "--with-bugurl=no "

    if variant == "linux" then
      if bootstrap then
         mkdir_p("#{sysroot}/include")
         mkdir_p("#{sysroot}/usr/include")
         b.run(:cmd => "../configure " +
                       "#{configure_common} " +
                       "--disable-libgomp " +
                       "--disable-threads " +
                       "--enable-languages=c " +
                       "#{sysroot_option}",
               :env => env)
      else
        b.run(:cmd => "../configure " +
                      "#{configure_common} " +
                      "#{threads} " +
                      "--with-libelf=#{kalray_internal} " +
                      "--enable-lto " +
                      "--enable-languages=#{languages} " +
                      "#{gomp} " +
                      "#{newlib} " +
                      "--enable-libstdcxx-allocator=malloc " +
                      "--with-headers=#{sysroot}/include " +
                      "--with-headers=#{sysroot}/sys-include " +
                      "#{sysroot_option}",
              :env => env)
      end
    elsif bootstrap then
      rm_rf("#{toolroot}/#{variant_dir}/sys-include")
      mkdir_p("#{toolroot}/#{variant_dir}/sys-include")
      #b.run("touch #{toolroot}/#{variant_dir}/sys-include/limits.h")

      b.run(:cmd => "../configure  " +
                    "#{configure_common} " +
                    "--disable-libgomp " +
                    "--disable-threads " +
                    "--enable-languages=c " +
                    "#{sysroot_option} " +
                    "#{newlib}",
            :env => env)
      # --target=#{target} non?
    else

      b.run(:cmd => "../configure  " +
                    "#{configure_common} " +
                    "#{threads} " +
                    "--with-libelf=#{kalray_internal} " +
                    "--enable-lto " +
                    "--enable-languages=#{languages} " +
                    "#{gomp} " +
                    "#{newlib}  " +
                    "--enable-sjlj-exceptions=yes " +
                    "--enable-libstdcxx-allocator=malloc " +
                    "#{sysroot_option}",
            :env => env)

      mkdir_p File.join(build_dir,variant_dir)
      mkdir_p "#{build_dir}/lib/gcc"
      ln_sf File.join(toolroot,variant_dir,"board"), File.join(build_dir,variant_dir)
      ln_sf File.join(toolroot,variant_dir,"core"), File.join(build_dir,variant_dir)
      ln_sf File.join(toolroot,variant_dir,include_path), File.join(build_dir,variant_dir)
      system "ln -sf #{toolroot}/lib/gcc/* #{build_dir}/lib/gcc"
    end
    if build_type == "Debug" then
      extra_flags = debug_flags
    else
      extra_flags = ""
    end
    b.run(:cmd => "unset LIBRARY_PATH CPATH C_INCLUDE_PATH PKG_CONFIG_PATH CPLUS_INCLUDE_PATH INCLUDE ; make all FAMDIR=#{family_prefix} ARCH=#{arch} #{extra_flags}", :env => env)
  end
end

b.target("clean" + target_variant) do
  build_dirs.zip(lib_options, flag_options).each do |build_dir, lib_opt, flag_opt|
    puts "========= Cleaning Linux in #{build_type} mode. =========\n"
    b.run("rm -rf #{build_dir}")
  end
end

b.target("valid" + s_bootstrap + target_variant) do
    build_dirs.zip(lib_options, flag_options).each do |build_dir, lib_opt, flag_opt|
        cd "#{build_dir}/gcc"
        if s_bootstrap != "" then
            # pthread-init-2.c checks for the ANSI-ness of the headers.
            b.valid(:cmd => "make check-gcc DEJAGNU=../../../../valid/hudson/site.exp RUNTESTFLAGS=\"--target_board=#{arch}-sim dg.exp=gcc.dg/pthread-init-2.c\"", :env => env)
            b.valid("{ grep FAIL testsuite/gcc/gcc.sum || exit 0; } && cat testsuite/gcc/gcc.log && exit 1");
        elsif variant == "elf"
            march_valid_list.each do |march|
                b.valid(:cmd => "make check-gcc DEJAGNU=../../../../valid/hudson/site.exp RUNTESTFLAGS=\"--target_board=#{march}-sim k1.exp\"", :env => env)
                b.valid("{ grep FAIL testsuite/gcc/gcc.sum || exit 0; } && cat testsuite/gcc/gcc.log && exit 1");
            end
        end
    end
end

b.target("install" + s_bootstrap + target_variant) do
  build_dirs.zip(lib_options, flag_options).each do |build_dir, lib_opt, flag_opt|
    cd build_dir
    if ("#{build_type}" == "Release") then
      b.run(:cmd => "make install-strip-only-on-host", :env => env)
    else
      b.run(:cmd => "make install", :env => env)
    end

    # Install for packaging in install_prefix
    if(not bootstrap) then
#      if ("#{build_type}" == "Release") then
#        b.run(:cmd => "make prefix=#{install_prefix} install-strip-only-on-host", :env => env)
#      else
#        b.run(:cmd => "make prefix=#{install_prefix} install", :env => env)
#      end
      b.run(:cmd => "make prefix=#{install_prefix} install-gcc", :env => env)
      b.run(:cmd => "make prefix=#{install_libstdcpp_prefix} install-target-libstdc++-v3", :env => env)
      b.run(:cmd => "make prefix=#{install_libgomp_prefix} install-target-libgomp", :env => env) if(variant == "nodeos")
      b.run(:cmd => "make prefix=#{install_info_prefix} install-libcpp", :env => env) if(variant == "elf" or variant == "linux")
      b.run(:cmd => "make prefix=#{install_libgfortran_prefix} install-strip-target-libgfortran", :env => env)
    end

    install_paths.each do |install_path|
      cd "#{install_path}/bin"
      case arch 
      when "k1" then
        case variant
        when "elf" then
          b.run("ln -sf #{arch}-gcc #{arch}cc")
          mkdir_p("#{install_path}/#{arch}-elf/include/")
          b.run("cp -f #{gcc_path}/gcc/config/k1/builtin_k1.h #{install_path}/#{arch}-elf/include/")
        when "rtems" then
          mkdir_p("#{install_path}/#{arch}-rtems/include/")
          b.run("cp -f #{gcc_path}/gcc/config/k1/builtin_k1.h #{install_path}/#{arch}-rtems/include/")
        when "linux" then
          # Patch install_prefix with subdirectory from sysroot: usually k1-linux
          # Only applied here for headers and libs...
          install_path = File.join(install_prefix,sysroot_sd) if(install_path == install_prefix)

          mkdir_p("#{install_path}/usr/include/")
          mkdir_p("#{install_path}/#{core}/usr/include/")
          b.run("cp -f #{gcc_path}/gcc/config/k1/builtin_k1.h #{install_path}/usr/include/")
          b.run("cp -f #{gcc_path}/gcc/config/k1/builtin_k1.h #{install_path}/#{core}/usr/include/")
        when "nodeos" then
          b.run("ln -sf #{arch}-gcc #{arch}cc")
          mkdir_p("#{install_path}/#{arch}-nodeos/include/")
          b.run("cp -f #{gcc_path}/gcc/config/k1/builtin_k1.h #{install_path}/#{arch}-nodeos/include/")      
        else
          raise "Unknown variant '#{variant}'"
        end

        if(variant != "linux") then
          # In all cases, copy also to k1tools
          mkdir_p("#{install_path}/k1tools/include/")
          b.run("cp -f #{gcc_path}/gcc/config/k1/builtin_k1.h #{install_path}/include/")
        end
      end
    end
  end
end

b.target("package" + s_bootstrap + target_variant) do
  b.logtitle = "Report for GCC packaging, arch = #{arch}"

  next if(bootstrap)

  # libstdc++ package
  cd install_libstdcpp_prefix
    
  gcc_name = "#{pkg_prefix_name}libstdc++-#{variant}"
  gcc_tar  = "#{gcc_name}.tar"
  b.run("tar cf #{gcc_tar} ./*")
  tar_package = File.expand_path(gcc_tar)
  
  depends = []
    
  tools_version = options["version"]
  (version,buildID) = tools_version.split("-")
  release_info = b.release_info(version,buildID)

  if(variant != "linux") then
    b.add_depend("#{pkg_prefix_name}newlib-#{variant}", depends, release_info)
    b.add_depend("#{pkg_prefix_name}bsp-#{variant}", depends, release_info)
  end
  
  package_description = "#{arch.upcase} GCC libstdc++ #{variant} package.\n"
  
  pinfo = b.package_info(gcc_name, release_info,
                         package_description, depends)
  pinfo.license = "GPLv3+ and GPLv3+ with exceptions and GPLv2+ with exceptions and LGPLv2+ and BSD"
  
  b.create_package(tar_package, pinfo)
  b.run("rm #{tar_package}")

  if(variant == "nodeos") then
    # libgomp package
    cd install_libgomp_prefix
    
    gcc_name = "#{pkg_prefix_name}libgomp-#{variant}"
    gcc_tar  = "#{gcc_name}.tar"
    b.run("tar cf #{gcc_tar} ./*")
    tar_package = File.expand_path(gcc_tar)
    
    depends = []
    
    tools_version = options["version"]
    (version,buildID) = tools_version.split("-")
    release_info = b.release_info(version,buildID)
    
    b.add_depend("#{pkg_prefix_name}newlib-#{variant}", depends, release_info)
    b.add_depend("#{pkg_prefix_name}bsp-#{variant}", depends, release_info)
    
    package_description = "#{arch.upcase} GCC libgomp #{variant} package.\n"
    
    pinfo = b.package_info(gcc_name, release_info,
                           package_description, depends)
    pinfo.license = "GPLv3+ and GPLv3+ with exceptions and GPLv2+ with exceptions and LGPLv2+ and BSD"
    
    b.create_package(tar_package, pinfo)
    b.run("rm #{tar_package}")
  end

  if(variant == "elf" or variant == "linux") then
    # GCC info package
    cd install_info_prefix
    
    gcc_name = "#{pkg_prefix_name}gcc-info"
    gcc_tar  = "#{gcc_name}.tar"
    b.run("tar cf #{gcc_tar} ./*")
    tar_package = File.expand_path(gcc_tar)
    
    depends = []
    
    tools_version = options["version"]
    (version,buildID) = tools_version.split("-")
    release_info = b.release_info(version,buildID)
    
    package_description = "#{arch.upcase} GCC info package.\n"
    
    pinfo = b.package_info(gcc_name, release_info,
                           package_description, depends)
    pinfo.license = "GPLv3+ and GPLv3+ with exceptions and GPLv2+ with exceptions and LGPLv2+ and BSD"
    
    b.create_package(tar_package, pinfo)
    b.run("rm #{tar_package}")
  end

  if(variant != "linux") then
    # libgfortran package
    cd install_libgfortran_prefix
    
    gcc_name = "#{pkg_prefix_name}libgfortran-#{variant}"
    gcc_tar  = "#{gcc_name}.tar"
    b.run("tar cf #{gcc_tar} ./*")
    tar_package = File.expand_path(gcc_tar)
    
    depends = []
    
    tools_version = options["version"]
    (version,buildID) = tools_version.split("-")
    release_info = b.release_info(version,buildID)
  
    b.add_depend("#{pkg_prefix_name}newlib-#{variant}", depends, release_info)
    b.add_depend("#{pkg_prefix_name}bsp-#{variant}", depends, release_info)
    
    package_description = "#{arch.upcase} GCC libgfortran #{variant} package.\n"
    
    pinfo = b.package_info(gcc_name, release_info,
                           package_description, depends)
    pinfo.license = "GPLv3+ and GPLv3+ with exceptions and GPLv2+ with exceptions and LGPLv2+ and BSD"
    
    b.create_package(tar_package, pinfo)
    b.run("rm #{tar_package}")
  end

  # GCC package
  cd install_prefix
    
  gcc_name = "#{pkg_prefix_name}gcc-#{variant}"
  gcc_tar  = "#{gcc_name}.tar"
  b.run("tar cf #{gcc_tar} ./*")
  tar_package = File.expand_path(gcc_tar)
    
  depends = []
    
  tools_version = options["version"]
  (version,buildID) = tools_version.split("-")
  release_info = b.release_info(version,buildID)
  
  if(variant == "linux") then
    b.add_depend("#{pkg_prefix_name}uClibc", depends, release_info)
  else
    b.add_depend("#{pkg_prefix_name}libgfortran-#{variant}", depends, release_info)
    b.add_depend("#{pkg_prefix_name}newlib-#{variant}", depends, release_info)
    b.add_depend("#{pkg_prefix_name}bsp-#{variant}", depends, release_info)
  end
  b.add_depend("#{pkg_prefix_name}libstdc++-#{variant}", depends, release_info)
  b.add_depend("#{pkg_prefix_name}gbu-#{variant}", depends, release_info)
  if(variant == "nodeos") then
    b.add_depend("#{pkg_prefix_name}libgomp-#{variant}", depends, release_info)
  end
  b.add_depend("#{pkg_prefix_name}gcc-info", depends, release_info)
  
  package_description = "#{arch.upcase} GCC #{variant} package.\n"
  package_description += "This package provides Gnu Compiler Collection for MPPA (#{variant})."
  
  pinfo = b.package_info(gcc_name, release_info,
                         package_description, depends)
  pinfo.license = "GPLv3+ and GPLv3+ with exceptions and GPLv2+ with exceptions and LGPLv2+ and BSD"
  
  b.create_package(tar_package, pinfo)
  b.run("rm #{tar_package}")
end

b.target("gcc") do
  if( arch == "k1" )
    build_dirs.zip(lib_options, flag_options).each do |build_dir, lib_opt, flag_opt|
      kalray_internal_lib=File.join(kalray_internal,lib_opt)
      cflags = ""
      cflags = "CFLAGS=-m32" if `file #{toolroot}/bin/#{arch}-elf-gcc | grep "ELF 64"` == "" 

      with_extra_flags="--with-mpc-lib=#{mpc_prefix}/#{lib_opt} --with-mpfr-lib=#{mpfr_prefix}/#{lib_opt} --with-gmp-lib=#{gmp_prefix}/#{lib_opt}"

      # Validation in the valid project
      b.create_goto_dir! "#{gcc_path}/#{arch}_build"
      # Create build directories. This configure line should be kept in sync 
      # with above.
      b.run(:cmd => "#{cflags} ../configure " +
                    "#{with_extra_flags} " +
                    "--prefix=#{toolroot} " +
                    "#{configure_with} " +
                    "--disable-bootstrap " +
                    "--disable-shared " +
                    "--enable-multilib " +
                    "--disable-libmudflap " +
                    "--disable-libssp " +
                    "--enable-checking=release " +
                    "--with-system-zlib " +
                    "--enable-__cxa_atexit " +
                    "--target=#{variant_dir} " +
                    "--enable-threads " +
                    "--with-libelf=#{kalray_internal} " +
                    "--with-system-zlib " +
                    "--program-prefix=#{arch}- " +
                    "--with-bugurl=no " +
                    "--enable-lto " +
                    "--enable-languages=c,c++,fortran " +
                    "--with-newlib " +
                    "--enable-libstdcxx-allocator=malloc " +
                    "#{sysroot_option} ",
            :env => env)
      b.run(:cmd => "make configure-gcc", :env => env)
      cd "gcc"

      march_valid_list.each do |march|
        # run the testsuite
        b.valid(:cmd => "ulimit -s unlimited && make check DEJAGNU=../../../../valid/hudson/site.exp RUNTESTFLAGS=\"--target_board=#{march}-sim\"", :env => env)

        # check for changes (and in particular for regression)
        b.valid(:cmd => "../../contrib/dg-cmp-results.sh -v -v #{arch}-sim ../../valid/hudson/testsuite-refs/#{march}/gcc.sum testsuite/gcc/gcc.sum | (grep \\\\-\\>FAIL || true) > testsuite/gcc.diff", :env => env)
        b.valid(:cmd => "../../contrib/dg-cmp-results.sh -v -v #{arch}-sim ../../valid/hudson/testsuite-refs/#{march}/g++.sum testsuite/g++/g++.sum | (grep \\\\-\\>FAIL || true) >> testsuite/gcc.diff", :env => env)
        b.valid(:cmd => "../../contrib/dg-cmp-results.sh -v -v #{arch}-sim ../../valid/hudson/testsuite-refs/#{march}/gfortran.sum testsuite/gfortran/gfortran.sum | (grep \\\\-\\>FAIL || true) >> testsuite/gcc.diff", :env => env)
        b.valid("echo 'Regression for arch : #{march}'; diff -u /dev/null testsuite/gcc.diff")
        b.valid("mkdir -p testsuite-refs")
        b.valid("mv testsuite testsuite-refs/#{march}")
      end
    end
  else raise "Unknown target: #{arch}"
  end
end

b.target("libgomp_valid") do
  if(arch == "k1")
    cd "#{gcc_path}/k1_nodeos_Debug_build_64/k1-nodeos/libgomp"
    b.valid(:cmd => "make check DEJAGNU=../../../../valid/hudson/site-nodeos.exp RUNTESTFLAGS=\"--target_board=k1-sim\" ", :env => env);
    b.silent("{ grep FAIL testsuite/libgomp.sum || exit 0; } && echo -e \"######################################\nYou've broken libgomp\n######################################\" && exit -1")
  else raise "Unknown target: #{arch}"
  end
end

b.target("libgomp") do
  if( arch == "k1" )
    b.create_goto_dir! "#{gcc_path}/#{arch}_libgomp_build"

    env["DEJAGNU"] = "#{gcc_path}/valid/hudson/site-nodeos.exp"
    b.valid(:cmd => "runtest  --tool libgomp --srcdir #{gcc_path}/libgomp/testsuite/ --target_board=k1-sim", :env => env)
  else raise "Unknown target: #{arch}"
  end
end

b.target("copyright_check") do
    printf "No copyright check for gcc\n"
    #FileUtils.mkdir_p "#{workspace}/copyright_check"
    #CopyrightChecker.check_copyright([gcc_path], "#{gcc_path}/.copyright_ignore", "#{workspace}/copyright_check/gcc")
end

b.launch
