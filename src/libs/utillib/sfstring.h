#pragma once
/*-------------------------------------------------------------------------------------------
 * QuickBlocks - Decentralized, useful, and detailed data from Ethereum blockchains
 * Copyright (c) 2018 Great Hill Corporation (http://quickblocks.io)
 *
 * This program is free software: you may redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version. This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details. You should have received a copy of the GNU General
 * Public License along with this program. If not, see http://www.gnu.org/licenses/.
 *-------------------------------------------------------------------------------------------*/
#include <string>
#include "basetypes.h"

namespace qblocks {

    //--------------------------------------------------------------------
    typedef std::string string_q;

    //---------------------------------------------------------------------------------------
    // We use 'extract' instead of the string class's substr to protect against throws
    extern string_q extract(const string_q& haystack, size_t pos, size_t len = string::npos);

    //---------------------------------------------------------------------------------------
    extern void replace    (string_q& target, const string_q& what, const string_q& with);
    extern void replaceAll (string_q& target, const string_q& what, const string_q& with);
    extern void replaceAny (string_q& target, const string_q& list, const string_q& with);
    extern void replaceReverse(string_q& target, const string_q& what, const string_q& with);
    extern void reverse    (string_q& target);

    //---------------------------------------------------------------------------------------
    inline string_q substitute(const string_q& strIn, const string_q& what, const string_q& with) {
        string_q ret;
        ret = strIn;
        replaceAll(ret, what, with);
        return ret;
    }

    //--------------------------------------------------------------------
    inline bool operator%(const string_q& str1, const string_q& str2) {
        return !strcasecmp(str1.c_str(), str2.c_str());
    }

    //--------------------------------------------------------------------
    inline bool operator%(const string_q& str1, const char *str2) {
        return !strcasecmp(str1.c_str(), str2);
    }

    //--------------------------------------------------------------------
    inline bool operator%(const char *str1, const string_q& str2) {
        return !strcasecmp(str1, str2.c_str());
    }

    //--------------------------------------------------------------------
    inline bool operator%(const string_q& str1, char ch) {
        string_q str2;
        str2 = ch;
        return !strcasecmp(str1.c_str(), str2.c_str());
    }

    //--------------------------------------------------------------------
    inline bool operator%(char ch, const string_q& str2) {
        string_q str1;
        str1 = ch;
        return !strcasecmp(str1.c_str(), str2.c_str());
    }

    //--------------------------------------------------------------------
    extern size_t  countOf        (const string_q& haystack, char needle);
    extern bool    startsWith     (const string_q& haystack, const string_q& needle);
    extern bool    startsWith     (const string_q& haystack, char ch);
    extern bool    endsWith       (const string_q& haystack, const string_q& needle);
    extern bool    endsWith       (const string_q& haystack, char ch);
    extern bool    endsWithAny    (const string_q& haystack, const string_q& needle);
    extern bool    startsWithAny  (const string_q& haystack, const string_q& needle);
    extern bool    contains       (const string_q& haystack, const string_q& needle);
    extern bool    contains       (const string_q& haystack, char ch);
    extern bool    containsI      (const string_q& haystack, const string_q& needle);

    //--------------------------------------------------------------------
    extern string_q toLower       (const string_q& in);
    extern string_q toUpper       (const string_q& in);
    extern string_q toProper      (const string_q& in);

    //--------------------------------------------------------------------
    extern string_q padRight      (const string_q& str, size_t len, char p = ' ');
    extern string_q padLeft       (const string_q& str, size_t len, char p = ' ');
    extern string_q padCenter     (const string_q& str, size_t len, char p = ' ');

    //--------------------------------------------------------------------
    extern string_q trim          (const string_q& str, char c = ' ');
    extern string_q trimTrailing  (const string_q& str, char c = ' ');
    extern string_q trimLeading   (const string_q& str, char c = ' ');
    extern string_q trimWhitespace(const string_q& str, const string_q& add = "");

    //--------------------------------------------------------------------
    extern string_q nextTokenClearReverse(string_q& str, char token);
    extern string_q nextTokenClear       (string_q& str, char delim, bool doClear = true);
    extern string_q snagFieldClear       (string_q& str, const string_q& tagName, const string_q& defVal = "");

    //---------------------------------------------------------------------------------------
    inline string_q getEnvStr(const char* name) {
        char *sss = getenv(name);
        return ( sss ? string_q(sss) : string_q(""));
    }

}  // namespace qblocks
