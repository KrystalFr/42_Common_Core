/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Brain.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 04:29:19 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/20 17:21:27 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Brain.hpp"

Brain::Brain(void)
{
	for (int i = 0; i < 100; ++i)
        this->ideas[i] = "";
    std::cout << "Brain: Constructor called" << std::endl;
}

Brain::Brain(const Brain& other)
{
	for (int i = 0; i < 100; ++i)
        this->ideas[i] = other.ideas[i];
    std::cout << "Brain: Copy constructor called" << std::endl;
}

Brain& Brain::operator=(const Brain& other)
{
	if (this != &other)
    {
        for (int i = 0; i < 100; ++i)
            this->ideas[i] = other.ideas[i];
    }
    std::cout << "Brain: Copy assignment operator called" << std::endl;
    return *this;
}

Brain::~Brain(void)
{
    std::cout << "Brain: Destructor called" << std::endl;
}

std::string Brain::getIdea(int id) const
{
	if (id < 0 || id >= 100)
		return std::string();
	return this->ideas[id];
}

void Brain::setIdea(int id, const std::string& idea)
{
	if (id < 0 || id >= 100)
		return;
	this->ideas[id] = idea;
}
