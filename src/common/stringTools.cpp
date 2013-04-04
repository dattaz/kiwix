/*
 * Copyright 2011 Emmanuel Engelhart <kelson@kiwix.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU  General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "stringTools.h"

#ifndef __ANDROID__

/* Prepare integer for display */
std::string kiwix::beautifyInteger(const unsigned int number) {
  std::stringstream numberStream;
  numberStream << number;
  std::string numberString = numberStream.str();

  signed int offset = numberString.size() - 3;
  while (offset > 0) {
    numberString.insert(offset, ",");
    offset -= 3;
  }

  return numberString;
}

/* Split string in a token array */
std::vector<std::string> kiwix::split(const std::string & str,
                                      const std::string & delims=" *-")
{
  std::string::size_type lastPos = str.find_first_not_of(delims, 0);
  std::string::size_type pos = str.find_first_of(delims, lastPos);
  std::vector<std::string> tokens;

  while (std::string::npos != pos || std::string::npos != lastPos)
    {
      tokens.push_back(str.substr(lastPos, pos - lastPos));
      lastPos = str.find_first_not_of(delims, pos);
      pos     = str.find_first_of(delims, lastPos);
    }

  return tokens;
}

std::vector<std::string> kiwix::split(const char* lhs, const char* rhs){
  const std::string m1 (lhs), m2 (rhs);
  return split(m1, m2);
}

std::vector<std::string> kiwix::split(const char* lhs, const std::string& rhs){
  return split(lhs, rhs.c_str());
}

std::vector<std::string> kiwix::split(const std::string& lhs, const char* rhs){
  return split(lhs.c_str(), rhs);
}

std::string kiwix::removeAccents(const std::string &text) {
  ucnv_setDefaultName("UTF-8");
  UErrorCode status = U_ZERO_ERROR;
  Transliterator *removeAccentsTrans = Transliterator::createInstance("Lower; NFD; [:M:] remove; NFC", UTRANS_FORWARD, status);
  UnicodeString ustring = UnicodeString(text.c_str());
  removeAccentsTrans->transliterate(ustring);
  delete removeAccentsTrans;
  std::string unaccentedText;
  ustring.toUTF8String(unaccentedText);
  return unaccentedText;
}

std::string kiwix::ucFirst (const std::string &word) {
  if (word.empty())
    return "";

  std::string ucFirstWord;
  UnicodeString firstLetter = UnicodeString(word.substr(0, 1).c_str());
  UnicodeString ucFirstLetter = firstLetter.toUpper();
  ucFirstLetter.toUTF8String(ucFirstWord);
  ucFirstWord += word.substr(1);
  return ucFirstWord;
}

std::string kiwix::lcFirst (const std::string &word) {
  if (word.empty())
    return "";

  std::string ucFirstWord;
  UnicodeString firstLetter = UnicodeString(word.substr(0, 1).c_str());
  UnicodeString ucFirstLetter = firstLetter.toLower();
  ucFirstLetter.toUTF8String(ucFirstWord);
  ucFirstWord += word.substr(1);
  return ucFirstWord;
}

void kiwix::printStringInHexadecimal(UnicodeString s) {
  std::cout << std::showbase << std::hex;
  for (int i=0; i<s.length(); i++) {
    char c = (char)((s.getTerminatedBuffer())[i]);
    if (c & 0x80)
      std::cout << (c & 0xffff) << " ";
    else
      std::cout << c << " ";
  }
  std::cout << std::endl;
}

void kiwix::printStringInHexadecimal(const char *s) {
  std::cout << std::showbase << std::hex;
  for (char const* pc = s; *pc; ++pc) {
    if (*pc & 0x80)
      std::cout << (*pc & 0xffff);
    else
      std::cout << *pc;
    std::cout << ' ';
  }
  std::cout << std::endl;
}

void kiwix::stringReplacement(std::string& str, const std::string& oldStr, const std::string& newStr) {
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos) {
    str.replace(pos, oldStr.length(), newStr);
    pos += newStr.length();
  }
}

// Urlencode
//based on javascript encodeURIComponent()

std::string char2hex( char dec )
{
  char dig1 = (dec&0xF0)>>4;
  char dig2 = (dec&0x0F);
  if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
  if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
  if ( 0<= dig2 && dig2<= 9) dig2+=48;
  if (10<= dig2 && dig2<=15) dig2+=97-10;

  std::string r;
  r.append( &dig1, 1);
  r.append( &dig2, 1);
  return r;
}

std::string kiwix::urlEncode(const std::string &c) {
  std::string escaped="";
  int max = c.length();
  for(int i=0; i<max; i++)
    {
      if ( (48 <= c[i] && c[i] <= 57) ||//0-9
	   (65 <= c[i] && c[i] <= 90) ||//abc...xyz
	   (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
	   (c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'')
	   )
        {
	  escaped.append( &c[i], 1);
        }
      else
        {
	  escaped.append("%");
	  escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
        }
    }
  return escaped;
}

std::string kiwix::urlDecode(const std::string &SRC) {
  std::string ret;
  char ch;
  int i, ii;
  for (i=0; i<SRC.length(); i++) {
    if (int(SRC[i])==37) {
      sscanf(SRC.substr(i+1,2).c_str(), "%x", &ii);
      ch=static_cast<char>(ii);
      ret+=ch;
      i=i+2;
    } else {
      ret+=SRC[i];
    }
  }
  return (ret);
}

#else

std::string kiwix::ucFirst (const std::string &word) {
  if (word.empty())
    return "";

	std::string new_string = word;
	new_string[0] = toupper(new_string[0]);

  return new_string;
}

std::string kiwix::lcFirst (const std::string &word) {
  if (word.empty())
    return "";

	std::string new_string = word;
	new_string[0] = tolower(new_string[0]);

  return new_string;
}

#endif