/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PresidentialPardonForm.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 13:25:57 by krfranco          #+#    #+#             */
/*   Updated: 2025/11/24 16:07:14 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/PresidentialPardonForm.hpp"

PresidentialPardonForm::PresidentialPardonForm(const std::string& t)
	: AForm("PresidentialPardonForm", 25,5)
	, target(t)
{
	// std::cout << "PresidentialPardonForm: Default constructor called" << std::endl;
}

PresidentialPardonForm::PresidentialPardonForm(const PresidentialPardonForm& other)
	:AForm(other)
	, target(other.target)
{
	// std::cout << "PresidentialPardonForm: Copy constructor called" << std::endl;
}

PresidentialPardonForm& PresidentialPardonForm::operator=(const PresidentialPardonForm& other)
{
	// std::cout << "PresidentialPardonForm: Copy assignment operator called" << std::endl;
	if (this != &other)
	{
		AForm::operator=(other);
	}
	return *this;
}

PresidentialPardonForm::~PresidentialPardonForm()
{
	// std::cout << "PresidentialPardonForm: Destructor called" << std::endl;
}

void PresidentialPardonForm::executeAction() const
{
	std::cout << this->target << " has been pardonned by Zaphod Beeblebrox" << std::endl;
}
