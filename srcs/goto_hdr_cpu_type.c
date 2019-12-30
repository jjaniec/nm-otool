/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   goto_hdr_cpu_type.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/12/21 15:13:52 by joffreyjani       #+#    #+#             */
/*   Updated: 2019/12/31 00:12:14 by jjaniec          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_nm.h>

t_ft_nm_hdrinfo			*goto_hdr_cpu_type(t_ft_nm_hdrinfo *hdr_list, cpu_type_t target_type)
{
	t_ft_nm_hdrinfo		*h;

	h = hdr_list;
	dprintf(2, "first: %u\n", h->cpu_subtype);
	while (h && \
		(h->cpu_type != target_type || \
		!((h->cpu_subtype & 0xf) == CPU_SUBTYPE_X86_64_ALL || \
		(h->cpu_subtype & 0xf) == CPU_SUBTYPE_X86_ALL)))
	{
		dprintf(2, "Next: %u\n", h->cpu_subtype);
		h = h->next;
	}
	return (h);
}
