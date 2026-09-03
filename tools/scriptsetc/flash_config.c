// SFCX_CONFIG							>>17&3 	>>4&3
// corona 4G?    FlashConfig: C0462002  3       0
Corona (16MB)  FlashConfig: 00043000
Trinity (16MB) FlashConfig: 00023010  1		1
Jasper (256MB) FlashConfig: 008A3020  1		2
Jasper (512MB) FlashConfig: 00AA3020  1		2
Jasper (16MB)  FlashConfig: 00023010  1		1
preJas (64MB)  FlashConfig: 01198030  0		3
preJas (16MB)  FlashConfig: 01198010  0		1

Corona (16MB)  FlashConfig: 00043000
Trinity (16MB) FlashConfig: 00023010
Jasper (256MB) FlashConfig: 008A3020
Jasper (512MB) FlashConfig: 00AA3020
Jasper (16MB)  FlashConfig: 00023010
preJas (64MB)  FlashConfig: 01198030
preJas (16MB)  FlashConfig: 01198010
Jasper (64MB)  FlashConfig: 00023030 << inferred

unsigned int SFC_init(void)
{
	unsigned int config;

	sfc.initialized = 0;
	sfc.meta_type = 0;
	sfc.page_sz = 0x200;
	sfc.meta_sz = 0x10;
	sfc.page_sz_phys = sfc.page_sz + sfc.meta_sz;

	config = SFC_readreg(SFC_CONFIG);

	switch ((config >> 17) & 0x03)
	{
	case 0: // Small block original SFC (pre jasper)
		sfc.meta_type = 0;
		sfc.blocks_per_lg_block = 8;

		switch ((config >> 4) & 0x3)
		{
		case 0: // Unsupported 8MB?
//            g_Console.Format(" ! SFCX: Unsupported Type A-0\n");
			return 1;

			//sfc.block_sz = 0x4000; // 16 KB
			//sfc.size_blocks = 0x200;
			//sfc.size_bytes = sfc.size_blocks << 0xE;
			//sfc.size_usable_fs = 0xXXX;
			//sfc.addr_config = 0x07BE000 - 0x4000;

		case 1: // 16MB
			sfc.block_sz = 0x4000; // 16 KB
			sfc.size_blocks = 0x400;
			sfc.size_bytes = sfc.size_blocks << 0xE;
			sfc.size_usable_fs = 0x3E0;
			//sfc.addr_config = 0x0F7C000 - 0x4000;
			sfc.addr_config = (sfc.size_usable_fs - 0x04) * sfc.block_sz;
			break;

		case 2: // 32MB
			sfc.block_sz = 0x4000; // 16 KB
			sfc.size_blocks = 0x800;
			sfc.size_bytes = sfc.size_blocks << 0xE;
			sfc.size_usable_fs = 0x7C0;
			//sfc.addr_config = 0x1EFC000 - 0x4000;
			sfc.addr_config = (sfc.size_usable_fs - 0x04) * sfc.block_sz;
			break;

		case 3: // 64MB
			sfc.block_sz = 0x4000; // 16 KB
			sfc.size_blocks = 0x1000;
			sfc.size_bytes = sfc.size_blocks << 0xE;
			sfc.size_usable_fs = 0xF80;
			//sfc.addr_config = 0x3DFC000 - 0x4000;
			sfc.addr_config = (sfc.size_usable_fs - 0x04) * sfc.block_sz;
			break;
		}
		break;
	case 1: // New SFC/Southbridge: Codename "Panda"?
	case 2: // New SFC/Southbridge: Codename "Panda" v2?
		switch ((config >> 4) & 0x3)
		{
		case 0: 
			
			if(((config >> 17) & 0x03) == 0x01)
			{
				// Unsupported
				sfc.meta_type = 0;
				//g_Console.Format(" ! SFCX: Unsupported Type B-0\n");
				return 2;
			}
			else
			{
				sfc.meta_type = 1;
				sfc.block_sz = 0x4000; // 16 KB
				sfc.size_blocks = 0x400;
				sfc.size_bytes = sfc.size_blocks << 0xE;
				sfc.blocks_per_lg_block = 8;
				sfc.size_usable_fs = 0x3E0;
				//sfc.addr_config = 0x0F7C000 - 0x4000;
				sfc.addr_config = (sfc.size_usable_fs - 0x04) * sfc.block_sz;
				break;
			}

		case 1: 

			if(((config >> 17) & 0x03) == 0x01)
			{
				// Small block 16MB setup
				sfc.meta_type = 1;
				sfc.block_sz = 0x4000; // 16 KB
				sfc.size_blocks = 0x400;
				sfc.size_bytes = sfc.size_blocks << 0xE;
				sfc.blocks_per_lg_block = 8;
				sfc.size_usable_fs = 0x3E0;
				//sfc.addr_config = 0x0F7C000 - 0x4000;
				sfc.addr_config = (sfc.size_usable_fs - 0x04) * sfc.block_sz;
				break;
			}
			else
			{
				// Small block 64MB setup
				sfc.meta_type = 1;
				sfc.block_sz = 0x4000; // 16 KB
				sfc.size_blocks = 0x1000;
				sfc.size_bytes = sfc.size_blocks << 0xE;
				sfc.blocks_per_lg_block = 8;
				sfc.size_usable_fs = 0xF80;
				//sfc.addr_config = 0x3DFC000 - 0x4000;
				sfc.addr_config = (sfc.size_usable_fs - 0x04) * sfc.block_sz;
				break;
			}

		case 2: // Large Block: Current Jasper 256MB and 512MB
			sfc.meta_type = 2;
			sfc.block_sz = 0x20000; // 128KB
			sfc.size_bytes = 0x1 << (((config >> 19) & 0x3) + ((config >> 21) & 0xF) + 0x17);
			sfc.size_blocks = sfc.size_bytes >> 0x11;
			sfc.blocks_per_lg_block = 1;
			sfc.size_usable_fs = 0x1E0;
			//sfc.addr_config = 0x3BE0000 - 0x4000;
			sfc.addr_config = (sfc.size_usable_fs - 0x04) * sfc.block_sz;
			break;

		case 3: // Large Block: Future or unknown hardware
			sfc.meta_type = 2;
			sfc.block_sz = 0x40000; // 256KB
			sfc.size_bytes = 0x1 << (((config >> 19) & 0x3) + ((config >> 21) & 0xF) + 0x17);
			sfc.size_blocks = sfc.size_bytes >> 0x12;
			sfc.blocks_per_lg_block = 1;
			sfc.size_usable_fs = 0xF0;
			//sfc.addr_config = 0x3BC0000 - 0x4000;
			sfc.addr_config = (sfc.size_usable_fs - 0x04) * sfc.block_sz;
			break;
		}
		break;

	default:
//        g_Console.Format(" ! SFCX: Unsupported Type\n");
		return 3;
	}

	sfc.len_config = sfc.block_sz * 0x04; //4 physical blocks

	sfc.pages_in_block = sfc.block_sz / sfc.page_sz;
	sfc.block_sz_phys = sfc.pages_in_block * sfc.page_sz_phys;

	sfc.size_pages = sfc.size_bytes / sfc.page_sz;
	sfc.size_blocks = sfc.size_bytes / sfc.block_sz;

	sfc.size_bytes_phys = sfc.block_sz_phys * sfc.size_blocks;
	sfc.size_mb = sfc.size_bytes >> 20;

#if 0
	g_Console.Format("   config register     = %08X\n", config);

	g_Console.Format("   sfc:meta_type       = %08X\n", sfc.meta_type);

	g_Console.Format("   sfc:page_sz         = %08X\n", sfc.page_sz);
	g_Console.Format("   sfc:meta_sz         = %08X\n", sfc.meta_sz);
	g_Console.Format("   sfc:page_sz_phys    = %08X\n", sfc.page_sz_phys);

	g_Console.Format("   sfc:pages_in_block  = %08X\n", sfc.pages_in_block);
	g_Console.Format("   sfc:block_sz        = %08X\n", sfc.block_sz);
	g_Console.Format("   sfc:block_sz_phys   = %08X\n", sfc.block_sz_phys);

	g_Console.Format("   sfc:size_mb         = %dMB\n", sfc.size_mb);
	g_Console.Format("   sfc:size_bytes      = %08X\n", sfc.size_bytes);
	g_Console.Format("   sfc:size_bytes_phys = %08X\n", sfc.size_bytes_phys);

	g_Console.Format("   sfc:size_pages      = %08X\n", sfc.size_pages);
	g_Console.Format("   sfc:size_blocks     = %08X\n", sfc.size_blocks);
	g_Console.Format("\n");

#endif

	//should later do this:
	//assert(sfc.meta_type == page_0[0x71]);

	//meta_type = SFC_read_metadata_type();
	//if (meta_type == -1){
	//	//g_Console.Format(" ! SFCX: Meta Type detection error\n");
	//	return 4;
	//}

	//if (meta_type != sfc.meta_type){
	//	//g_Console.Format(" ! SFCX: Meta Type detection difference\n");
	//	//g_Console.Format(" ! SFCX: expecting type: '%d' detected: '%d'\n", sfc.meta_type, meta_type);
	//	sfc.meta_type = meta_type;
	//}

	sfc.initialized = 1;
	return 0;
}