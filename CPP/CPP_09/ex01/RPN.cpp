/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:19:26 by krfranco          #+#    #+#             */
/*   Updated: 2026/06/04 13:06:52 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

bool is_op(const std::string &token)
{
	return (token == "+" || token == "-" || token == "*" || token == "/");
}

int calculate(const std::string op, int left, int right)
{
	if (op == "+")
		return left + right;
	else if (op == "-")
		return left - right;
	else if (op == "*")
		return left * right;
	else if (op == "/")
	{
		if (right == 0)
			throw std::runtime_error("division by zero");
		return left / right;
	}
	else
		throw std::runtime_error("unknown operator");
}

int RPN(const std::string &input)
{
	std::stack<int, std::list<int> > st;
	std::stringstream ss(input);
	std::string token;
	
	while (ss >> token)
	{
		if (token.size() == 1 && std::isdigit(token[0]))
		{
			st.push(token[0] - '0');
		}
		else if(is_op(token))
		{
			if (st.size() < 2)
    			throw std::runtime_error("bad expression");
			int right = st.top(); st.pop();
			int left = st.top(); st.pop();
			st.push(calculate(token, left, right));
		}
		else
			throw std::runtime_error("invalid token");
	}
	
	if (st.size() > 1)
		throw std::runtime_error("bad expression");
	int res = st.top();
	return res;
}