#!/usr/bin/env ruby

success = true

gcc_prefix = ARGV[0] or raise "Must specify as first argument gcc source path" 
gcc_prefix = File.join(gcc_prefix,"gcc","config","k1")

File.exists?(gcc_prefix) or raise "Directory #{gcc_prefix} does not exist."

builtin_k1 = File.expand_path("builtin_k1.h",gcc_prefix)
k1_c = File.expand_path("k1.c",gcc_prefix)

builtins = {}
builtin_struct = Struct.new(:name,:file,:line,:gcc_proto,:c_proto)

gcc_types = {
  "void"               => "VOID",
  "void *"             => "voidPTR",
  "const void *"       => "constVoidPTR",
  "float"              => "floatSF",
  "double"             => "floatDF",
  "char"               => "intQI",
  "unsigned char"      => "uintQI",
  "short"              => "intHI",
  "unsigned short"     => "uintHI",
  "int"                => "intSI",
  "unsigned int"       => "uintSI",
  "long long"          => "intDI",
  "unsigned long long" => "uintDI",
  "__int128"           => "intTI",
  "unsigned __int128"  => "uintTI",
}

File.open(builtin_k1, "r") do |file|
  line_nb = 1
  file.each do |line|
    if(line =~ /^extern/ and not line =~ /\"C\"/) then
      fields = line.chomp().gsub(/^extern (.+) __builtin_k1_([^\s]+)\s*\((.+)\).+/,'\1,\2,\3').split(",")
      name = fields[1]
      add_builtin = ["ADD_K1_BUILTIN(#{name.upcase}"]
      add_builtin.push "\"#{name.downcase}\""
      fields.each.with_index do |field,i|
        if(i != 1) then
          type = field.gsub(/^\s+/,"")
          # Removing variable names
          type.gsub!(/^(.*)void\s+\*.*/,'\1void *')
          type.gsub!(/^([^_]*int*).*/,'\1')
          type.gsub!(/^(.*short).*/,'\1')
          type.gsub!(/^(.*char).*/,'\1')
          type.gsub!(/^.*float.*/,"float")
          type.gsub!(/^.*double.*/,"double")
          type.gsub!(/^(.*)long\s+long.*/,'\1long long')
          # Do not consider void parameter...
          next if(i > 1 and type == "void")
          if(not gcc_types.has_key?(type)) then
            STDERR.puts "#{builtin_k1}:#{line_nb}: error: Unknown gcc type '#{type}' for:\n\t#{line}"
            success = false
          else
            add_builtin.push gcc_types[type]
          end
        end
      end
      if(builtins.has_key?(name)) then
        STDERR.puts "#{builtin_k1}:#{line_nb}: error: Builtin '#{name}' defined several times"
        success = false
      end
      builtins[name] = builtin_struct.new(name,builtin_k1,line_nb,add_builtin.join(",") + ");",line.chomp())
    end
    line_nb = line_nb + 1
  end
end

k1_c_builtins = {}

File.open(k1_c, "r") do |file|
  line_nb = 1
  file.each do |line|
    if(line =~ /^[\s\t]*ADD_K1_BUILTIN/) then
      line = line.chomp().gsub(/\s*/,"")
      name = line.split(",")[1].gsub(/"/,"")
      k1_c_builtins[name] = builtin_struct.new(name,k1_c,line_nb,line,"")
      if(not builtins.has_key?(name)) then
        STDERR.puts "#{k1_c}:#{line_nb}: error: Not supported GCC builtin (builtin_k1.h): #{name} -> #{line}"
        success = false
      else
        builtin = builtins[name]
        if(builtin.gcc_proto != line) then
          STDERR.puts "#{builtin.file}:#{builtin.line}: error: Bad prototype for builtin: #{name}\nbuiltin_k1.h: #{builtin.gcc_proto}, k1.c: #{line}"
          success = false
        end
      end
    end
    line_nb = line_nb + 1
  end
end

k1_c_builtins.each do |name,builtin|
  if(not builtins.has_key?(name)) then
    STDERR.puts "#{builtin.file}:#{builtin.line}: error: Not supported GCC builtin (k1.c): #{name} -> #{builtin.gcc_proto}"
    success = false
  end
end

exit success
