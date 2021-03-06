//
// MapConfiguration.cpp
//
// $Id: //poco/1.3/Util/src/MapConfiguration.cpp#1 $
//
// Library: Util
// Package: Configuration
// Module:  MapConfiguration
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
// 
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#include "TitaniumMapConfiguration.h"
#include <set>


namespace Poco {
namespace Util {


TitaniumMapConfiguration::TitaniumMapConfiguration()
{
}


TitaniumMapConfiguration::~TitaniumMapConfiguration()
{
}


void TitaniumMapConfiguration::clear()
{
	_map.clear();
}


bool TitaniumMapConfiguration::getRaw(const std::string& key, std::string& value) const
{
	StringMap::const_iterator it = _map.find(key);
	if (it != _map.end())
	{
		value = it->second;
		return true;
	}
	else return false;
}


void TitaniumMapConfiguration::setRaw(const std::string& key, const std::string& value)
{
	_map[key] = value;
}


bool TitaniumMapConfiguration::removeProperty(const std::string& key)
{
	StringMap::iterator it = _map.find(key);
	if (it != _map.end())
	{
		_map.erase(it);
		return true;
	}
	else return false;
}


void TitaniumMapConfiguration::enumerate(const std::string& key, Keys& range) const
{
	std::set<std::string> keys;
	std::string prefix = key;
	if (!prefix.empty()) prefix += '.';
	std::string::size_type psize = prefix.size();
	for (StringMap::const_iterator it = _map.begin(); it != _map.end(); ++it)
	{
		if (it->first.compare(0, psize, prefix) == 0)
		{
			std::string subKey;
			std::string::size_type end = it->first.find('.', psize);
			if (end == std::string::npos)
				subKey = it->first.substr(psize);
			else
				subKey = it->first.substr(psize, end - psize);
			if (keys.find(subKey) == keys.end())
			{
				range.push_back(subKey);
				keys.insert(subKey);
			}
		}
	}
}


TitaniumMapConfiguration::iterator TitaniumMapConfiguration::begin() const
{
	return _map.begin();
}


TitaniumMapConfiguration::iterator TitaniumMapConfiguration::end() const
{
	return _map.end();
}


} } // namespace Poco::Util
