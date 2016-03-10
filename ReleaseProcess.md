In this document, we will simulate the release of an hypothetical version 1.0.42 from the trunk. Note that despite looking rather Unixish (and only tested on Linux and Mac OS X, at the moment), this process touches some files needed for the Windows part of the build (notably, `VisualC++/quadra.rc`).

  * Update the version number.

> This is done in `include/version.h`, which is where scripts will extract it from. Don't forget to commit the change!

  * Tag the release:

```
svn copy https://quadra.googlecode.com/svn/trunk/quadra https://quadra.googlecode.com/svn/tags/quadra-1.0.42
```

  * Export the package

> In a temporary directory, run the following command:

```
svn export https://quadra.googlecode.com/svn/tags/quadra-1.0.42
```

  * Prepare the package

```
cd quadra-1.0.42
autoheader
make dist
svn log -v https://quadra.googlecode.com/svn/tags/quadra-1.0.42
```

> This step will prepare not only the Autoconf parts of the build system, but also `quadra.rc` and various README and RPM spec files.

  * Archive and compress the package

```
cd ..
tar cf quadra-1.0.42.tar quadra-1.0.42
gzip -9 quadra-1.0.42.tar
```

  * Test the package

```
rm -rf quadra-1.0.42
tar zxvf quadra-1.0.42.tar.gz
cd quadra-1.0.42
./configure
make
```

  * Upload the archive

  * Announce the release
    * Post a [blog](http://quadragame.blogspot.com/) entry
    * Send a message to the [quadra-announce](http://groups.google.com/group/quadra-announce) group
    * Update the [Freshmeat entry](http://freshmeat.net/projects/quadra/)
    * Update the [Linux Game Tome entry](http://happypenguin.org/show?Quadra)