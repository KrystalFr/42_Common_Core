/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Intern.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/03 17:01:31 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/03 17:57:16 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INTERN_HPP
#define INTERN_HPP

# include <string>
# include <iostream>

class AForm;
class Intern
{
	private:
		AForm* makeShrub(const std::string& target) const;
		AForm* makeRobot(const std::string& target) const;
		AForm* makePres(const std::string& target) const;
		
	public:
		Intern();
		Intern(const Intern& other);
		Intern& operator=(const Intern& other);
		~Intern();
		AForm* makeForm(const std::string& formName, const std::string& target) const;
};

#endif