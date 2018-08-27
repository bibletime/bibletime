README for BibleTime [![Build Status](https://travis-ci.org/bibletime/bibletime.svg?branch=master)](https://travis-ci.org/bibletime/bibletime)[![Packaging status](https://repology.org/badge/tiny-repos/bibletime.svg)](https://repology.org/metapackage/bibletime)
====================

BibleTime is a Bible study application based on the Sword library and
Qt toolkit.

## INSTALLATION

You can just download and install BibleTime from
[github](https://github.com/bibletime/bibletime/releases/latest).
The following requirements are needed only if you want to develop BibleTime.

### BUILD REQUIREMENTS
 - A C++11 compiler.
 - QT 5+ (http://www.qt.io)
 - Sword 1.7.0+ (http://crosswire.org/sword)
 - CLucene 0.9.16a+ (http://clucene.sf.net)
 - cmake 2.6.0+ (http://cmake.org)

### DOCUMENTATION GENERATION REQUIREMENTS (OPTIONAL)
 - po4a
 - xsltproc, docbook-xml, docbook-xsl

These packages are used by BT committers to prepare
the documentation for the Handbook, Howto and the
website.  This tool set can be used by translators to
generate, test and validate their work.
There is no need to rely on these packages to bundle
BibleTime for distribution as the compiled documentation
is in the source tree ready for installation.

### PROCEDURE

In the toplevel directory of this package, you can find
the scripts `build-debug.sh` and `build-release.sh`.

Run `build-debug.sh` to build BibleTime in debug mode.
It will build in the directory `./build/` and install to
`./build/install/`. You can run it just there, you do not
have to install it to `/usr/`.

For a real installation, run `build-release.sh`. This will
also build in `./build/`, but then install to `/usr/local`.


## FEEDBACK

Please report any bugs your find to
[our issue tracker](https://github.com/bibletime/bibletime/issues)
or see [the BibleTime homepage](http://www.bibletime.info/) for additional information.

## DOCUMENTATION

You can find information about BibleTime, the installation
and its usage in the following places:
 - http://www.bibletime.info/
 - The BibleTime handbook available in the Help menu of the BibleTime application.
 - There's a Bible study tutorial in the help menu.

May God, our Lord, use BibleTime for his glory.

The BibleTime developers
