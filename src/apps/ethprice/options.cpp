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
#include "etherlib.h"
#include "options.h"

//---------------------------------------------------------------------------------------------------
CParams params[] = {
    CParams("-at:<timestamp>",                 "Report the price since nearest five minutes to the given timestamp"),
    CParams("-current",                        "Report on the current price (i.e. -at:now)"),
    CParams("-freshen",                        "Freshen database (append new data)"),
    CParams("-period:<5|15|30|*120|240|1440>", "Display prices in this increment. One of [5|15|30|120*|240|1440]"),
    CParams("-pair:<val>",                     "Which price pair to freshen or list (see Poloniex)"),
    CParams("",                                "Freshen and/or display Ethereum price data and other purposes.\n"),
};
size_t nParams = sizeof(params) / sizeof(CParams);

//---------------------------------------------------------------------------------------------------
bool COptions::parseArguments(string_q& command) {

    if (!standardOptions(command))
        return false;

    Init();
    while (!command.empty()) {

        string_q arg  = nextTokenClear(command, ' ');
        // do not collapse
        if (arg == "-c" || arg == "--current")
            arg = "-a:now";
        string_q orig = arg;

        if (arg == "-f" || arg == "--freshen") {
            freshen = true;

        } else if (startsWith(arg, "-a:") || startsWith(arg, "--at:")) {
            arg = substitute(substitute(orig, "-a:", ""), "--at:", "");
            if (arg == "now") {
                at = toTimestamp(Now());
            } else {
                at = toTimestamp(arg);
                if (!isUnsigned(arg))
                    return usage("Timestamp expected: " + orig);
            }

        } else if (startsWith(arg, "-p:") || startsWith(arg, "--period:")) {
            arg = substitute(substitute(orig, "-p:", ""), "--period:", "");
            freq = toLongU(arg);
            if (!isUnsigned(arg) || freq % 5)
                return usage("Positive multiple of 5 expected: " + orig);

        } else if (startsWith(arg, "--pair:")) {
            arg = substitute(orig, "--pair:", "");
            source.pair = arg;

        } else {
            if (!builtInCmd(arg)) {
                return usage("Invalid option: " + arg);
            }
        }
    }

    if (!fileExists(source.getDatabasePath())) {
        if (verbose)
            cerr << "Establishing price database cache.\n";
        establishFolder(source.getDatabasePath());
    }

    return true;
}

//---------------------------------------------------------------------------------------------------
void COptions::Init(void) {
    paramsPtr = params;
    nParamsRef = nParams;

    freshen = false;
    freq = 120;
    at = 0;
}

//---------------------------------------------------------------------------------------------------
COptions::COptions(void) {
    needsOption = true;
    Init();
}

//--------------------------------------------------------------------------------
string_q COptions::postProcess(const string_q& which, const string_q& str) const {

    if (which == "options") {
        // return substitute(
        //       substitute(str, "address_list block_list", "<address> [address...] [block...]"), "-l|", "-l fn|");

    } else if (which == "notes" && (verbose || COptions::isReadme)) {

        // string_q ret;
        // ret += "[{addresses}] must start with '0x' and be forty characters long.\n";
        // ret += "[{block_list}] may be a space-separated list of values, a start-end range, a [{special}], "
        //          "or any combination.\n";
        // ret += "This tool retrieves information from the local node or the ${FALLBACK} node, if configured "
        //          "(see documentation).\n";
        // ret += "If the queried node does not store historical state, the results are undefined.\n";
        // ret += "[{special}] blocks are detailed under " + cTeal + "[{whenBlock --list}]" + cOff + ".\n";
        // return ret;
    }
    return str;
}
