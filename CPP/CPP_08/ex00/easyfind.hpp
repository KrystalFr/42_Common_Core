/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   easyfind.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:06:32 by krfranco          #+#    #+#             */
/*   Updated: 2026/04/07 14:13:32 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EASY_FIND_HPP
#define EASY_FIND_HPP

#include <algorithm>
#include <exception>

class notFound: public std::exception
{
	public:
		virtual const char* what() const throw()
		{
			return "Value not found";
		}
};

template <typename T>
typename T::iterator easyfind(T &container, int val)
{
	//iterator ~= pointer
	typename T::iterator it = std::find(container.begin(), container.end(), val);
	if (it == container.end())
		throw notFound();
	return it;
}

template <typename T>
typename T::const_iterator easyfind(const T&container, int val)
{
	typename T::const_iterator it = std::find(container.begin(), container.end(), val);
	if (it == container.end())
		throw notFound();
	return it;
}

#endif