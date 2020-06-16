#include "parser.hpp"

#include <sstream>

#include "malformed_json_error.hpp"

using namespace jsondom;

void parser::feed(utki::span<const char> data){
	for(auto i = data.begin(), e = data.end(); i != e; ++i){
		ASSERT(!this->state_stack.empty())
		switch(this->state_stack.back()){
			case state::idle:
				this->parse_idle(i, e);
				break;
			case state::object:
				this->parse_object(i, e);
				break;
			case state::array:
				this->parse_array(i, e);
				break;
		}
		if(i == e){
			return;
		}
	}
}

void parser::parse_idle(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e){
	for(; i != e; ++i){
		ASSERT(this->buf.empty())
		switch(*i){
			case '\n':
				++this->line;
			case ' ':
			case '\r':
			case '\t':
				break;
			case '[':
				this->state_stack.push_back(state::array);
				this->on_array_start();
				return;
			case '{':
				this->state_stack.push_back(state::object);
				this->on_object_start();
				return;
			default:
				std::stringstream ss;
				ss << "unexpected character '" << *i << "' encountered while in idle state";
				throw malformed_json_error(ss.str());
		}
	}
}

void parser::parse_object(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e){
	for(; i != e; ++i){
		ASSERT(this->buf.empty())
		switch(*i){
			case '\n':
				++this->line;
			case ' ':
			case '\r':
			case '\t':
				break;
			case '}':
				this->state_stack.pop_back();
				this->on_object_end();
				return;
			// default:
			// 	std::stringstream ss;
			// 	ss << "unexpected character '" << *i << "' encountered while in idle state";
			// 	throw malformed_json_error(ss.str());
		}
	}
}

void parser::parse_array(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e){
	for(; i != e; ++i){
		ASSERT(this->buf.empty())
		switch(*i){
			case '\n':
				++this->line;
			case ' ':
			case '\r':
			case '\t':
				break;
			// case '[':
			// 	this->cur_state = state::array;
			// 	this->on_array_start();
			// 	return;
			// case '{':
			// 	this->cur_state = state::object;
			// 	this->on_object_start();
			// 	return;
			// default:
			// 	std::stringstream ss;
			// 	ss << "unexpected character '" << *i << "' encountered while in idle state";
			// 	throw malformed_json_error(ss.str());
		}
	}
}
