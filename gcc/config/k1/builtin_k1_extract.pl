#!/usr/bin/perl -w
# usage: perl builtin_k1_extract.pl builtin_k1.h

sub match {
	my $text = shift;
	$text =~ /^const void \*/ && return "constVoidPTR";
	$text =~ /^char/ && return "intQI";
	$text =~ /^double/ && return "floatDF";
	$text =~ /^float/ && return "floatSF";
	$text =~ /^int/ && return "intSI";
	$text =~ /^long long/ && return "intDI";
	$text =~ /^__int128/ && return "intTI";
	$text =~ /^long/ && return undef; #"intSI";
	$text =~ /^short/ && return "intHI";
	$text =~ /^signed char/ && return "intQI";
	$text =~ /^signed int/ && return "intSI";
	$text =~ /^signed long long/ && return "intDI";
	$text =~ /^signed __int128/ && return "intTI";
	$text =~ /^signed long/ && return undef; #"intSI";
	$text =~ /^signed short/ && return "intHI";
	$text =~ /^signed/ && return "intSI";
	$text =~ /^unsigned char/ && return "uintQI";
	$text =~ /^unsigned __int128/ && return "uintTI";
	$text =~ /^unsigned int/ && return "uintSI";
	$text =~ /^unsigned long long/ && return "uintDI";
	$text =~ /^unsigned long/ && return undef; #"uintSI";
	$text =~ /^unsigned short/ && return "uintHI";
	$text =~ /^unsigned/ && return "uintSI";
	$text =~ /^void \*/ && return "voidPTR";
	$text =~ /^void/ && return "VOID";
	return undef;
}

my %table;

while (<>) {
	if (s/^extern\s+// && s/\s*__builtin_k1_(\w+)\s*/|$1/) {
		chomp; s/[\(\),]\s*/|/g; s/;\s*.*//;
		my @params =  split '\|';
		my $return = shift @params;
		my $name = shift @params;
		unshift @params, $return;
		#print "BUILTIN($name)\t"; 
		my @signature;
		map {
			my $item = &match($_);
			if (defined $item) {
				#print STDERR "matched $item\n";
				if (@signature && $item eq "VOID") { }
				else { push @signature, $item; }
			} else {
				print STDERR "unmatched type $_\n";
			}
		} @params;
		#print "PARAMS(@params)\n"; 
		my $NAME = uc($name);
		my $LIST = join ',', @signature;
		#print "ADD_K1_BUILTIN($NAME,\"$name\",$LIST);\n";
		print STDERR "Builtin $NAME already defined\n" if defined $table{$NAME};
		$table{$NAME} = "ADD_K1_BUILTIN($NAME,\"$name\",$LIST);\n";
	}
}

foreach my $key (sort { $a cmp $b } keys %table) {
	print $table{$key};
}

