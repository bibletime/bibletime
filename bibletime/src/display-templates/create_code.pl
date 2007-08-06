#!/usr/bin/perl

use strict;
my $dir = $ARGV[0];

print "\nrunning create-code.pl to update built-in templates......\n";

my %names;

sub read_names() {
	my $dir = shift;
	open(IN, "< $dir/names.conf") || die "Cannot open $dir/names.conf";
	
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


&read_names($dir);
my $code = "";
foreach my $f (glob("$dir/*tmpl")) {
	my ($name, $html) = &extract_data( $f );
	$code .= "\tm_templateMap[ i18n(\"$name\") ] = \"$html\";\n" if $name;
}

open(OUT, "> template-init.cpp");
print OUT "void CDisplayTemplateMgr::init() {\n$code\n}\n";
close(OUT);

print "create-code.pl finished.\n\n";