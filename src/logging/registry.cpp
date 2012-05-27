/*
 * Copyright (C) 2011 Asymworks, LLC.  All Rights Reserved.
 *
 * Developed by: Asymworks, LLC <info@asymworks.com>
 * 				 http://www.asymworks.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal with the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimers.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimers in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the names of Asymworks, LLC, nor the names of its contributors
 *      may be used to endorse or promote products derived from this Software
 *      without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * WITH THE SOFTWARE.
 */

#include <cstring>
#include <map>
#include <vector>

#include "benthos/logbook/logging/logger.hpp"
#include "benthos/logbook/logging/record.hpp"
#include "benthos/logbook/logging/registry.hpp"

using namespace benthos::logbook::logging;

struct logger_tree_item
{
	logger *									_logger;
	std::map<std::string, logger_tree_item *>	children;

	logger_tree_item(logger * parent = 0)
		: _logger(parent), children()
	{
	}

	~logger_tree_item()
	{
		std::map<std::string, logger_tree_item *>::iterator it;
		for (it = children.begin(); it != children.end(); it++)
		{
			// Ensure the parent logger is still valid while its children
			// are being deleted
			logger * l = it->second->_logger;
			delete it->second;
			delete l;
		}
	}
};

/*
 * Implementation of the Logger Class
 */
class logger_impl: public logger
{
public:
	logger_impl(logger * parent, const std::string & name)
		: logger(parent, name)
	{
	}
};

// Root of the Logger Tree
struct logger_tree_item * _tree;

// Root Logger Pointer
logger * _root;

// Module Initializer
class _init
{
public:

	// Initialize the Logging Module
	_init()
	{
		_root = new logger_impl(0, std::string());
		_tree = new logger_tree_item(_root);

		_root->setLevel(level::WARNING);
	}

	// Shutdown the Logging Module
	~_init()
	{
		delete _tree;
		delete _root;
	}
};
_init __init;

std::vector<std::string> parse_dotted_path(const std::string & path)
{
	std::vector<std::string> r;

	const char * _p = path.c_str();
	char * p = strdup(_p);
	char * t = strtok(p, ".");
	while (t)
	{
		r.push_back(std::string(t));
		t = strtok(NULL, ".");
	}

	delete p;
	return r;
}

logger_tree_item * getChildOf(logger_tree_item * t, const std::string & name)
{
	std::map<std::string, logger_tree_item *>::const_iterator it;
	for (it = t->children.begin(); it != t->children.end(); it++)
	{
		if (strcasecmp(name.c_str(), it->first.c_str()) == 0)
			return it->second;
	}

	// DNE - create it
	std::string dp;
	if (t->_logger == _root)
		dp = name;
	else
		dp = t->_logger->name() + "." + name;

	logger_tree_item * i = new logger_tree_item(new logger_impl(t->_logger, dp));
	t->children[name] = i;
	return i;
}

logger * benthos::logbook::logging::getLogger(const std::string & name)
{
	if (name.empty())
		return _root;

	// Parse the Dotted-Name Path
	//FIXME: Use Boost::Tokenizer?

	logger_tree_item * i = _tree;
	std::vector<std::string> p = parse_dotted_path(name);
	std::vector<std::string>::const_iterator it;
	for (it = p.begin(); it != p.end(); it++)
	{
		i = getChildOf(i, * it);
	}

	return i->_logger;
}
