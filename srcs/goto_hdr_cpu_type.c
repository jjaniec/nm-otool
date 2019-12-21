/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   goto_hdr_cpu_type.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joffreyjaniec <joffreyjaniec@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/21 15:13:52 by joffreyjani       #+#    #+#             */
/*   Updated: 2019/12/21 15:14:09 by joffreyjani      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

t_ft_nm_hdrinfo			*goto_hdr_cpu_type(t_ft_nm_hdrinfo *hdr_list, cpu_type_t target_type)
{
	t_ft_nm_hdrinfo		*h;

	h = hdr_list;
	while (h && h->cpu_type != target_type)
		h = h->next;
	return (h);
}
