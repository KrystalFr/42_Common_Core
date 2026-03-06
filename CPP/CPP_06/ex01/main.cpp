/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/06 21:48:32 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/06 22:47:19 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/Serializer.hpp"

int main()
{
	Data original;
	original.data = 42;

	uintptr_t ser = Serializer::serialize(&original);
	Data *deser = Serializer::deserialize(ser);

	if (deser == &original)
	{
		std::cout << "Successfull Serialization:" << std::endl;
		std::cout << "original adress: " << &original << " original data: " << original.data << std::endl;
		std::cout << "serialised ptr: " << ser << std::endl;
		std::cout << "deserialised ptr: " << deser << " deserialised data: " << (*deser).data << std::endl;
	}
	else
	{
		std::cout << "Failed Serialization:" << std::endl;
		return 1;
	}
	return 0;
}