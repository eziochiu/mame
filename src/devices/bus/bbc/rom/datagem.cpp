// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/***************************************************************************

    Gemini DataGem ROM Carrier

***************************************************************************/

#include "emu.h"
#include "datagem.h"


//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(BBC_DATAGEM, bbc_datagem_device, "bbc_datagem", "Gemini DataGem ROM Carrier")


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  bbc_datagem_device - constructor
//-------------------------------------------------

bbc_datagem_device::bbc_datagem_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, BBC_DATAGEM, tag, owner, clock)
	, device_bbc_rom_interface(mconfig, *this)
	, m_bank(0)
{
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void bbc_datagem_device::device_start()
{
	/* register for save states */
	save_item(NAME(m_bank));
}

//-------------------------------------------------
//  decrypt_rom
//-------------------------------------------------

void bbc_datagem_device::decrypt_rom()
{
	uint8_t *rom = get_rom_base();

	for (int i = 0x0000; i < 0x6000; i++)
	{
		/* decrypt 16K and 8K ROM data lines */
		switch (rom[i] & 0x07)
		{
		case 0x01: case 0x06: rom[i] = bitswap<8>(rom[i], 7, 6, 5, 4, 3, 0, 1, 2); break;
		case 0x02: case 0x05: rom[i] = bitswap<8>(rom[i], 7, 6, 5, 4, 3, 2, 0, 1); break;
		case 0x03: case 0x04: rom[i] = bitswap<8>(rom[i], 7, 6, 5, 4, 3, 1, 2, 0); break;
		}

		/* decrypt additional 8K ROM data lines */
		if (i & 0x4000)
		{
			rom[i] = bitswap<8>(rom[i], 3, 6, 5, 4, 7, 2, 1, 0);
		}
	}
}

//-------------------------------------------------
//  read
//-------------------------------------------------

uint8_t bbc_datagem_device::read(offs_t offset)
{
	if (!machine().side_effects_disabled())
	{
		/* switching zones for DataGem */
		switch (offset & 0x3fff)
		{
		case 0x1ffe: m_bank = 1; break;
		case 0x1fff: m_bank = 0; break;
		}
	}

	if (offset & 0x2000)
	{
		return get_rom_base()[(offset & 0x3fff) + (m_bank << 13)];
	}
	else
	{
		return get_rom_base()[offset & 0x3fff];
	}
}
