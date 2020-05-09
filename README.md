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
 - QT 5.9+ (https://www.qt.io)
 - Sword 1.8.1+ (https://crosswire.org/sword)
 - CLucene 0.9.19+ (https://clucene.sf.net)
 - CMake 3.10+ (https://cmake.org)

### DOCUMENTATION GENERATION REQUIREMENTS (OPTIONAL)
 - po4a
 - xsltproc
 - docbook-xsl
 - docbook-xml
 - fop (for PDF generation)

These packages are only used by the build system when document generation and
installation is enabled. By default, all translations of the handbook and howto
documents are generated and installed. This can be changed using the following
options to CMake:

 * BUILD_HANDBOOK_HTML - whether to generate and install the HTML handbook
 * BUILD_HANDBOOK_HTML_LANGUAGES
       - list of language codes to use for HTML handbook, or empty for all
         languages
 * BUILD_HANDBOOK_PDF - whether to generate and install the PDF handbook
 * BUILD_HANDBOOK_PDF_LANGUAGES
       - list of language codes to use for PDF handbook, or empty for all
         languages
 * BUILD_HOWTO_HTML - whether to generate and install the HTML howto
 * BUILD_HOWTO_HTML_LANGUAGES
       - list of language codes to use for HTML howto, or empty for all
         languages
 * BUILD_HOWTO_PDF - whether to generate and install the PDF howto
 * BUILD_HOWTO_PDF_LANGUAGES
       - list of language codes to use for PDF howto, or empty for all languages

For example, passing `-DGENERATE-HOWTO_HTML_LANGUAGES=en;et` to the cmake
command to restricts generation and installation of howto HTML versions to the
English and Estonian translations only, and `-DBUILD_HOWTO_PDF=OFF` disables
generation of the howto in PDF format.


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
