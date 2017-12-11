/*
 * Mrtp: A simple raytracing tool.
 * Copyright (C) 2017  Mikolaj Feliks <mikolaj.feliks@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _PARSER_H
#define _PARSER_H

#include <string>
#include <bitset>
#include <list>
#include "motifs.hpp"
#include "entry.hpp"


#define MAX_LINES       8
#define MAX_TOKENS      4
#define MAX_COMPONENTS  (MAX_TOKENS - 1)

/* Custom data types. */
enum ParserCode_t {codeOK, codeUnknown, codeType, codeSize, codeMissing, 
    codeRepeated, codeFilename, codeValue, codeConflict};

enum ParserStatus_t {statusOK, statusFail};
enum ParserMode_t {modeOpen, modeRead};


class Parser {
    std::string  path_;
    ParserStatus_t  status_;

    std::list<Entry *> entries_;
    std::list<Entry *>::iterator currentEntry_; 

    /* Private methods. */
    ParserCode_t CreateEntry (std::string *entryLabel, 
                              std::string collect[][MAX_TOKENS],
                              unsigned int sizes[], 
                              unsigned int ncol, 
                              Entry *entry);

    ParserCode_t PushParameter (std::bitset<MAX_BITS> flags, 
                                std::string tokens[MAX_TOKENS], 
                                unsigned int size, 
                                Entry *entry);

public:
    Parser (std::string *path);
    ~Parser ();

    void Parse ();
    void StartQuery ();
    bool Query (Entry **entry);
    ParserStatus_t Check (unsigned int *nentries);
};

#endif /* !_PARSER_H */
