/*
    This file is part of "GPS clock" project.

    "GPS clock" is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Some parts of this project are written by 3-rd party vendors under a
    different license. See readme.txt for the complete list of 3-rd party
    source code.

    "GPS clock" is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with "GPS clock".  If not, see <http://www.gnu.org/licenses/>.
*/

/**
* @file usr_messages.c
* @brief Localized text messages.
* @author Oleksii Slabchenko, alexslabchenko@gmail.com
* @copyright Oleksii Slabchenko, 2014
*/

#include "usr_messages.h"

/// @brief Days of week
/// @details You should use your local one-byte encoding, the same as for @ref usr_font.c
/// @warning Pay attention that week starts from Monday
char msg_days[7][3] = {
    "Âñ\0", ///< Sunday
    "Ïí\0", ///< Monday
    "Âò\0", ///< Tuesday
    "Ñð\0", ///< Wednesday
    "×ò\0", ///< Thursday
    "Ïò\0", ///< Friday
    "Ñá\0"  ///< Saturday
};

/*
char msg_days[7][3] = {
    "Su\0", ///< Sunday
    "Mo\0", ///< Monday
    "Tu\0", ///< Tuesday
    "We\0", ///< Wednesday
    "Th\0", ///< Thursday
    "Fr\0", ///< Friday
    "Sa\0"  ///< Saturday
};
*/

/// @brief This message will be shown right after the start,
/// indicating that GPS module is searching for satellites.
char msg_search[] = "Ïîèñê";
//char msg_search[] = "Search";
