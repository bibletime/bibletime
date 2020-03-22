
Several color attributes have been removed from the *.css
style files and moved to the *.cmap files. There is a pattern
left in the *.css files to allow easy substitution of 
colors back into the style during BibleTime execution.

The *.cmap files contain both a light and dark set of 
the colors. Each color is specified by a pattern or key
and a hex rgb value. In the *.css files you will see
values specified by using the pattern with "#" at both
ends of the value. If the user changes from a light
style to a dark style, a restart of BibleTime must
occur to see the correct style.


Example

In css file:

jesuswords {
    color: #JESUS_WORDS_COLOR#;
}

Corresponding entry in cmap file:

[dark]
JESUS_WORDS_COLOR=#ff6060

[light]
JESUS_WORDS_COLOR=#9c2323

The cmap file uses the "ini" syntax used by QSettings.


For each <style>.css file, an optional <style>.cmap can
exist. If the cmap file does not exist, default colors
will be used for both the dark and light cases. Also,
if a color is not specified in the cmap file, the 
default will be used.

The follow patterns/colors are currently in uses:
FOREGROUND_COLOR - Text color
BACKGROUND_COLOR - Background of text
JESUS_WORDS_COLOR - Words that Jesus said
CROSSREF_COLOR - Used by cross references, footnotes, etc.

Other pattern/color entrys can be used. Pattern/color
entries apply to the BibleTime text windows and the MAG
window.

The patterns FOREGROUND_COLOR and BACKGROUND_COLOR have
default values the same as the color of the non-text 
parts of BibleTime such as the Bookshelf or menus.
These entries can left out of the cmap file so that
the text windows look like the other parts of BibleTime.

For a cmap file to work a css file with the same 
basename should exist in the same directory. There
are two locations these files can exist. The first
is the display-templates directory provide by BibleTime.
The second is $HOME/.bibletime/display-templates
directory.

