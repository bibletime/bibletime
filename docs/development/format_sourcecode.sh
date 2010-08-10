#!/bin/sh

#kr   	= Kernigham & Ritchie
# Identically to
#	-nbad -bap -bbo -nbc -br -brs -c33 -cd33 -ncdb -ce -ci4 -cli0
#      	-cp33 -cs -d0 -di1 -nfc1 -nfca -hnl -i4 -ip0 -l75 -lp -npcs
#      	-nprs -npsl -saf -sai -saw -nsc -nsob -nss
#nce  	=
#bbo	=

#indent -kr  -fc1 -nce -cdb -cli2 -cbi2 -ts4 -sc -l120 -ncs -cdw $@

echo "Formatting source code files...";
for f in $(find . -name \*.cpp -or -name \*.h); do
	echo "	$f";
	astyle --style=java -t -T --indent-cases --brackets=break-closing-headers $f;
	#Fix that line\\ntext is turned into line\n\\t\t\ttext
	perl -e 'undef $/; my $t=<>; $t =~ s/\\\n\t+/\\\n/g; print $t;' < $f > $f.new;
	rm -f $f.orig;
	mv $f.new $f;
done

echo "...done.";
