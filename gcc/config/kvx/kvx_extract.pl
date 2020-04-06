#!/usr/bin/perl -w
# usage: perl kvx_extract.pl kvx.c

my %table;

while (<>) {
	if (/^\s*ADD_KVX_BUILTIN\s*\(\s*(\w+)/) {
		my $NAME = $1;
		s/\s+//g;
		print STDERR "Builtin $NAME already defined\n" if defined $table{$NAME};
		$table{$NAME} = "$_\n";
	}
}

foreach my $key (sort { $a cmp $b } keys %table) {
	print $table{$key};
}

