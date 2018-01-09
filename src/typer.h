#pragma once

#include "token.h"
#include "ast.h"

class Typer {
public:
    const inline bool can_assign_this(const TokenType& first, const TokenType& second) const {
        if (first == second) return true;
        else if (is_numeric(first) && is_numeric(second)) return true;
        return false;
    }

	const inline KeywordType get_keyword_type(const std::string& keyword) const {
		for (auto& entry : keywords) if (entry.first == keyword) return entry.second;
		return KeywordType::INVALID;
	}

	const inline bool is_numeric(const TokenType& type) const {
		return type == TokenType::INT || type == TokenType::FLOAT;
	}

	const inline TokenType get_type_from_string(const std::string& checking) {
		if (this->is_token_boolean(checking)) return TokenType::BOOL;
		for (auto& entry : type_map) if (entry.first == checking) return entry.second;
		return TokenType::INVALID;
	}

	const inline TokenType get_token_type(const std::string& checking) {
		if (this->is_token_boolean(checking)) return TokenType::BOOL;
		for (auto& entry : token_map) if (entry.second == checking) return entry.first;
		return TokenType::IDENTIFIER;
	}

	const inline std::string get_native(const std::string& name) const {
		auto using_name = Util::replace(name, "native_", "");
		return native_map[builtin_map[using_name]];
	}

	const inline bool is_builtin(const std::string& name) const {
		for (auto& entry : builtin_map) if (entry.first == name) return true;
		return false;
	}

	const inline bool is_operator(const TokenType& type) {
		switch (type) {
			case TokenType::OP_ADD:
			case TokenType::OP_SUB:
			case TokenType::OP_MUL:
			case TokenType::OP_DIV:
				return true;
			default:
				return false;
		}
	}

	const inline bool is_conditional_operator(const TokenType& type) {
		return type == TokenType::ASSIGN;
	}

	const inline std::string conditional_operator_as_string(const TokenType& type) {
		switch (type) {
			case TokenType::ASSIGN:
				return "=";
			default:
				return "invalid";
		}
	}

	const inline bool string_to_bool(const std::string& converting) {
		return converting == "true" ? true : false;
	}

	const inline bool is_token_boolean(const std::string& checking) {
		return checking == "true" || checking == "false";
	}
};
