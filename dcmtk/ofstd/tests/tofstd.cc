/*
 *
 *  Copyright (C) 2002-2016, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  ofstd
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: test program for class OFStandard
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstd.h"

#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"

#define OFFILENAME_TO_OFSTRING(filename) \
    OFString(OFSTRING_GUARD(filename.getCharPointer()))

static void checkExists(const OFString& input, OFBool pathExists,
        OFBool fileExists, OFBool dirExists)
{
    OFBool path = OFStandard::pathExists(input);
    OFBool file = OFStandard::fileExists(input);
    OFBool dir  = OFStandard::dirExists(input);

    if (pathExists != path)
        OFCHECK_FAIL("pathExists(\"" << input << "\"): expected: " << pathExists << ", got: " << path);
    if (fileExists != file)
        OFCHECK_FAIL("fileExists(\"" << input << "\"): expected: " << fileExists << ", got: " << file);
    if (dirExists != dir)
        OFCHECK_FAIL("dirExists(\"" << input << "\"): expected: " << dirExists << ", got: " << dir);
}

static void checkPathHandling(const OFString& input, const OFString& normalized,
        const OFString& combined, const OFString& file = "file")
{
    OFString result;
    OFString slashFile;
    slashFile = PATH_SEPARATOR;
    slashFile += "file";

    OFStandard::normalizeDirName(result, input);
    OFCHECK_EQUAL(result, normalized);

    OFStandard::combineDirAndFilename(result, input, file);
    OFCHECK_EQUAL(result, combined);

    OFStandard::combineDirAndFilename(result, input, PATH_SEPARATOR + file);
    OFCHECK_EQUAL(result, slashFile);
}

OFTEST(ofstd_testPaths_1)
{
    OFString result;
    OFString sourceRoot;
    OFString input;
    OFString normalized;
    OFString combined;
    OFString pathSeparator;

    pathSeparator += PATH_SEPARATOR;
    sourceRoot = ".." + pathSeparator + "..";

    normalized = sourceRoot;
    input = normalized + pathSeparator;
    combined = input + "file";

    checkExists(input, OFTrue, OFFalse, OFTrue);
    checkPathHandling(input, normalized, combined);

    // Trailing slashes shouldn't matter
    input += pathSeparator + pathSeparator + pathSeparator;

    checkExists(input, OFTrue, OFFalse, OFTrue);
    checkPathHandling(input, normalized, combined);

    // Now check a non-existent file
    normalized = sourceRoot + pathSeparator + "does_not_exist";
    input = normalized;
    combined = input + pathSeparator + "file";

    checkExists(input, OFFalse, OFFalse, OFFalse);
    checkPathHandling(input, normalized, combined);

    // Check the working dir handling
    input = normalized = ".";
    combined = "." + pathSeparator + "file";

    checkExists(input, OFTrue, OFFalse, OFTrue);
    checkPathHandling(input, normalized, combined);

    // How is the empty string treated?
    input = "";

    checkExists(input, OFFalse, OFFalse, OFFalse);
    checkPathHandling(input, normalized, combined);

    // Check what happens to "///"
    normalized = pathSeparator;
    input = pathSeparator + pathSeparator + pathSeparator;
    combined = normalized + "file";

    // No checkExists() since Windows doesn't have "/"
    checkPathHandling(input, normalized, combined);

    // TODO FIXME
    // These tests have problems. They assume that they are run from the
    // source dir, but with cmake they are usually run from elsewhere.
#if 0
    // Now let's check if the ofstd dir exists
    normalized = sourceRoot + pathSeparator + "ofstd";
    input = normalized;
    combined = input + pathSeparator + "file";

    checkExists(input, OFTrue, OFFalse, OFTrue);
    checkPathHandling(input, normalized, combined);

    // There should be a INSTALL file somewhere
    normalized = sourceRoot + pathSeparator + "INSTALL";
    input = normalized;
    combined = input + pathSeparator + "file";

    checkExists(input, OFTrue, OFTrue, OFFalse);
    checkPathHandling(input, normalized, combined);
#endif

    // Now come some special tests for combineDirAndFilename
    input = pathSeparator + pathSeparator + "caesar" + pathSeparator + "share";
    OFStandard::combineDirAndFilename(result, input, "file");
    OFCHECK_EQUAL(result, input + pathSeparator + "file");

    OFStandard::combineDirAndFilename(result, input, pathSeparator + "file");
    OFCHECK_EQUAL(result, pathSeparator + "file");

    OFStandard::combineDirAndFilename(result, "", "file");
    OFCHECK_EQUAL(result, "." + pathSeparator + "file");

    OFStandard::combineDirAndFilename(result, "", "file", OFTrue /*allowEmptyDirName*/);
    OFCHECK_EQUAL(result, "file");

    OFStandard::combineDirAndFilename(result, "", ".");
    OFCHECK_EQUAL(result, ".");

    OFStandard::combineDirAndFilename(result, "..", ".");
    OFCHECK_EQUAL(result, "..");

    OFStandard::combineDirAndFilename(result, "", "");
    OFCHECK_EQUAL(result, ".");

    OFStandard::combineDirAndFilename(result, "", "", OFTrue /*allowEmptyDirName*/);
    OFCHECK_EQUAL(result, "");

    // hidden files/directories start with a "." (on Unix systems)
    OFStandard::combineDirAndFilename(result, "", ".hidden", OFTrue /*allowEmptyDirName*/);
    OFCHECK_EQUAL(result, ".hidden");
}

OFTEST(ofstd_testPaths_2)
{
    OFString result;
    OFString pathSeparator;

    pathSeparator += PATH_SEPARATOR;

    // Check getDirNameFromPath()
    OFStandard::getDirNameFromPath(result, "dirname" + pathSeparator + "filename");
    OFCHECK_EQUAL(result, "dirname");
    OFStandard::getDirNameFromPath(result, pathSeparator + "dirname" + pathSeparator);
    OFCHECK_EQUAL(result, pathSeparator + "dirname");
    OFStandard::getDirNameFromPath(result, "dirname");
    OFCHECK_EQUAL(result, "dirname");
    OFStandard::getDirNameFromPath(result, "dirname", OFFalse /*assumeDirName*/);
    OFCHECK_EQUAL(result, "");

    // Check getFilenameFromPath()
    OFStandard::getFilenameFromPath(result, "dirname" + pathSeparator + "filename");
    OFCHECK_EQUAL(result, "filename");
    OFStandard::getFilenameFromPath(result, pathSeparator + "dirname" + pathSeparator);
    OFCHECK_EQUAL(result, "");
    OFStandard::getFilenameFromPath(result, pathSeparator + "filename");
    OFCHECK_EQUAL(result, "filename");
    OFStandard::getFilenameFromPath(result, "filename");
    OFCHECK_EQUAL(result, "filename");
    OFStandard::getFilenameFromPath(result, "filename", OFFalse /*assumeFilename*/);
    OFCHECK_EQUAL(result, "");
}

OFTEST(ofstd_OFStandard_isReadWriteable)
{
    // TODO FIXME
    // Same as above, this assumes that it's called from the source dir which
    // isn't necessarily true with cmake.
#if 0
    OFCHECK_EQUAL(OFStandard::isReadable("tofstd.cc"), OFTrue);
    OFCHECK_EQUAL(OFStandard::isWriteable("tofstd.cc"), OFTrue);
#endif
    OFCHECK_EQUAL(OFStandard::isReadable("does_not_exist"), OFFalse);
    OFCHECK_EQUAL(OFStandard::isWriteable("does_not_exist"), OFFalse);
}

OFTEST(ofstd_OFStandard_appendFilenameExtension)
{
    OFFilename result;
    const char *nullPtr = NULL;

    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, "file", ".ext")), "file.ext");
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, "", ".ext")), ".ext");
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, "file", "")), "file");
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, "file", nullPtr)), "file");
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, nullPtr, "")), "");
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(OFStandard::appendFilenameExtension(result, nullPtr, nullPtr)), "");
}

OFTEST(ofstd_OFStandard_removeRootDirFromPathname)
{
    OFFilename result;
    const char *nullPtr = NULL;

    OFString testPath = "/root";
    testPath += PATH_SEPARATOR;
    testPath += "path";
    OFString resultPath;
    resultPath += PATH_SEPARATOR;
    resultPath += "path";

    OFCHECK(OFStandard::removeRootDirFromPathname(result, "/root", testPath, OFTrue /*allowLeadingPathSeparator*/).good());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), resultPath);
    OFCHECK(OFStandard::removeRootDirFromPathname(result, "/root", testPath, OFFalse /*allowLeadingPathSeparator*/).good());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), "path");

    OFCHECK(OFStandard::removeRootDirFromPathname(result, "/root", "/no_root/path").bad());
    OFCHECK(result.isEmpty());
    OFCHECK(OFStandard::removeRootDirFromPathname(result, "", "/root/path").good());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), "/root/path");
    OFCHECK(OFStandard::removeRootDirFromPathname(result, "/root", "").bad());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), "");
    OFCHECK(OFStandard::removeRootDirFromPathname(result, "", "").good());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), "");
    OFCHECK(OFStandard::removeRootDirFromPathname(result, nullPtr, "/path").good());
    OFCHECK_EQUAL(OFFILENAME_TO_OFSTRING(result), "/path");
    OFCHECK(OFStandard::removeRootDirFromPathname(result, "/root", nullPtr).bad());
    OFCHECK_EQUAL(result.getCharPointer(), nullPtr);
    OFCHECK(OFStandard::removeRootDirFromPathname(result, nullPtr, nullPtr).good());
}

OFTEST(ofstd_safeSubtractAndAdd)
{
  // --------------- Subtraction ----------------

  unsigned int a = 1;
  unsigned int b = 2;
  // check whether underflow occurs (it should)
  OFCHECK(OFStandard::safeSubtract(a, b, a) == OFFalse);
  // check whether a has not been modified
  OFCHECK_EQUAL(a, 1);

  a = OFnumeric_limits<unsigned int>::max();
  b = OFnumeric_limits<unsigned int>::max()-1;
  // check whether no underflow occurred (it shouldn't)
  OFCHECK(OFStandard::safeSubtract(a, b, a) == OFTrue);
  // check whether the result a was computed as expected
  OFCHECK_EQUAL(a, 1);

  // --------------- Addition ----------------

  a = OFnumeric_limits<unsigned int>::max()-1;
  b = OFnumeric_limits<unsigned int>::max()-1;
  // check whether overflow occurred (it should)
  OFCHECK(OFStandard::safeAdd(a, b, a) == OFFalse);
  // check whether a has not been modified
  OFCHECK_EQUAL(a, OFnumeric_limits<unsigned int>::max()-1);

  b = 1; // a still equals max-1
  // check whether no overflow occurred (it shouldn't)
  OFCHECK(OFStandard::safeAdd(a, b, a) == OFTrue);
  // check whether the result a was computed as expected.
  // dividing and then multiplying by 2 is required since max may be an
  // odd number so that max/2 is rounded to the floor number.
  OFCHECK_EQUAL(a, OFnumeric_limits<unsigned int>::max());
}
