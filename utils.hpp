/*
 *  Mrtp: A simple raytracing tool.
 *  Copyright (C) 2017  Mikolaj Feliks <mikolaj.feliks@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _UTILS_H
#define _UTILS_H

#include <cstddef>  /* NULL pointer. */
#include <fstream>
#include <sstream>
#include <string>

/*
 * Macros.
 */
#define IS_COMMENT(c) (c == '#')

#define IS_WHITE(c) ((c == ' ') || (c == '\t'))

#define IS_NOT_WHITE(c) ((c != ' ') && (c != '\t'))


extern bool TokenizeLine (const std::string *line, std::string *tokens,
    unsigned int *ntokens, unsigned int maxtokens);
extern bool ConvertTokens (const std::string *tokens, unsigned int ntokens, 
    double *out);
extern bool CheckFilename (std::string *in, std::string *ext, std::string *out);

#endif /* _UTILS_H */
