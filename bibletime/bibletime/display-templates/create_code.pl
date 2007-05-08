#!/usr/bin/perl

use strict;
use File::Compare;
use File::Copy;
use FindBin qw($RealBin);

my %names;

sub read_names() {
	open(IN, "< " . $RealBin . "/names.conf");
	
	while (<IN>) {
		my $line = $_;
		chomp $line;
		
		my ($filename, $description) = split("\t", $line);
		$names{ $filename } = $description;
	}	
	
	close(IN);
}

sub extract_data {
	my $file = shift || die;
	
	my $name;
	my $html;
	
	$file =~ /(?:.+\/)?(.+\..+)/;
	my $tmpl_name = $1 || $file;
	$name = $names{ $tmpl_name };
	
	open(IN, "< $file");	
	while( <IN> ) {
		my $line = $_;
		chomp($line);
		$line .= "\\\n";
		
		
		$line =~ s/"/\\"/g;
		$line =~ s/\s{2,}|\t/ /g; #replace spaces and tabs by space
		
		$html .= $line;
	}
	close(INT);
	
	
	print "Found $name\n" if ($name);
	return ($name, $html);
}

my $dir = $RealBin;
#print $dir."\n";

&read_names;
my $code = "";
foreach my $f (@ARGV) {
	my ($name, $html) = &extract_data( $f );
	
	if ($name) {
		$code .= "\tm_templateMap[ i18n(\"$name\") ] = \"$html\";\n"
	}
}


open(OUT, "> $RealBin/template-init.cpp-new");
print OUT "void CDisplayTemplateMgr::init() {\n$code\n}\n";
close(OUT);

if (-e "$RealBin/template-init.cpp" && compare("$RealBin/template-init.cpp","$RealBin/template-init.cpp-new")) { #differ
	move("$RealBin/template-init.cpp-new","$RealBin/template-init.cpp");
}
elsif (!-e "$RealBin/template-init.cpp") {
	move ("$RealBin/template-init.cpp-new","$RealBin/template-init.cpp");
}
else { #make sure to remove the -new file
	unlink("$RealBin/template-init.cpp-new");
}