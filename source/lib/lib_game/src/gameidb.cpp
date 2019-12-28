#include "gameidb.h"
#include "logging.h"
#include <random>

// You're possibly adding an extra 30 seconds to a build by enabling this, can we find a better fucking way?
#define COMPILE_ITEMDB  0

#if COMPILE_ITEMDB
	#include "gameidbgenerated.h"
#else
	item_database s_ItemDatabase = {};
#endif


static NVItemDB::_remote_lookup s_pCacheMissLookup = nullptr;
static bool s_bHasRandomised = false;
static std::mt19937 s_RNG;

item_group_map* NVItemDB::lookup_group(std::string form_name)
{
	auto group_it = s_ItemDatabase.find(form_name);
	if (group_it == s_ItemDatabase.end())
	{
		return nullptr;
	}

	return &(group_it->second);
}

void NVItemDB::Initialize(_remote_lookup lookup_fn)
{
	s_pCacheMissLookup = lookup_fn;

	for (auto group = s_ItemDatabase.begin(); group != s_ItemDatabase.end(); ++group)
	{
		DbgLog("[%s] Loading group '%s'", __FUNCTION__, (group->first).c_str());

		auto& group_map = group->second;

		for (auto item = group_map.begin(); item != group_map.end(); ++item)
		{
			TESForm* pForm = lookup_fn(item->second.form_id);
			if (pForm == nullptr)
			{
				DbgWarn("NVItemDB returned invalid pointer object, cache miss.");
				continue;
			}

			item->second.instance_ref = pForm;
		}
	}
}

NVItemDB::ItemRow* NVItemDB::lookup(std::uint32_t form_id)
{
	for (auto group = s_ItemDatabase.begin(); group != s_ItemDatabase.end(); ++group)
	{
		auto list_lookup = group->second.find(form_id);
		if (list_lookup != group->second.end())
		{
			if (s_pCacheMissLookup != nullptr && list_lookup->second.instance_ref == nullptr)
			{
				DbgWarn("NVItemDB cache miss lookup...");
				list_lookup->second.instance_ref = s_pCacheMissLookup(list_lookup->second.form_id);
			}

			return &(list_lookup->second);
		}
	}

	return nullptr;
}

// what the actual flying fuck is this
NVItemDB::ItemRow* NVItemDB::first_entry_in_ammo_list(std::uint32_t ammo_list_id)
{
	std::uint32_t ammo_id;
	switch (ammo_list_id)
	{
		// AmmoList10mm
	case 0x001537E6:
		ammo_id = 0x00004241;
		break;

		// AmmoList127mm
	case 0x001613D5:
		ammo_id = 0x001429CF;
		break;

		// AmmoList12Ga
	case 0x0015367F:
		ammo_id = 0x0008ECF5;
		break;

		// AmmoList20Ga
	case 0x0015367E:
		ammo_id = 0x000E86F2;
		break;

		// AmmoList22LR
	case 0x001537EB:
		ammo_id = 0x0007EA27;
		break;

		// AmmoList25mmGrenade
	case 0x001537EE:
		ammo_id = 0x00096C40;
		break;

		// AmmoList308
	case 0x001537E9:
		ammo_id = 0x0006B53C;
		break;

		// AmmoList357Magnum
	case 0x001537E3:
		ammo_id = 0x0008ED02;
		break;

		// AmmoList40mmGrenade
	case 0x00158308:
		ammo_id = 0x0007EA26;
		break;

		// AmmoList44Magnum
	case 0x001537E4:
		ammo_id = 0x0002937E;
		break;

		// AmmoList4570
	case 0x001537E5:
		ammo_id = 0x00121133;
		break;

		// AmmoList50MG
	case 0x001537E8:
		ammo_id = 0x0008ECFF;
		break;

		// AmmoList556mm
	case 0x001537EA:
		ammo_id = 0x00004240;
		break;

		// AmmoList5mm
	case 0x001537EC:
		ammo_id = 0x0006B53D;
		break;

		// AmmoList9mm
	case 0x001537E7:
		ammo_id = 0x0008ED03;
		break;

		// AmmoListElectronChargePack
	case 0x0015831B:
		ammo_id = 0x0006B53E;
		break;

		// AmmoListFlamerFuel
	case 0x00166F63:
		ammo_id = 0x00029371;
		break;

		// AmmoListMicroFusionCell
	case 0x0015831A:
		ammo_id = 0x00004485;
		break;

		// AmmoListMissile
	case 0x001537ED:
		ammo_id = 0x00029383;
		break;

		// AmmoListSmallEnergyCell
	case 0x00158316:
		ammo_id = 0x00020772;
		break;

	default:
		ammo_id = 0;
		return nullptr;
	}

	return lookup(ammo_id);
}

NVItemDB::ItemRow_Verbose NVItemDB::lookup_verbose(std::uint32_t form_id)
{
	for (auto group = s_ItemDatabase.begin(); group != s_ItemDatabase.end(); ++group)
	{
		auto list_lookup = group->second.find(form_id);
		if (list_lookup != group->second.end())
		{
			if (s_pCacheMissLookup != nullptr && list_lookup->second.instance_ref == nullptr)
			{
				DbgWarn("NVItemDB cache miss lookup...");
				list_lookup->second.instance_ref = s_pCacheMissLookup(list_lookup->second.form_id);
			}

			return{ group->first, &list_lookup->second };
		}
	}

	return{ "", nullptr };
}

NVItemDB::ItemRow_Verbose NVItemDB::lookup_verbose(std::string form_name)
{
	for (auto group = s_ItemDatabase.begin(); group != s_ItemDatabase.end(); ++group)
	{
		for (auto item_entry = group->second.begin(); item_entry != group->second.end(); ++item_entry)
		{
			if (item_entry->second.form_ref == form_name)
			{
				if (s_pCacheMissLookup != nullptr && item_entry->second.instance_ref == nullptr)
				{
					DbgWarn("NVItemDB cache miss lookup...");
					item_entry->second.instance_ref = s_pCacheMissLookup(item_entry->second.form_id);
				}

				return{ group->first, &item_entry->second };
			}
		}
	}

	return{ "", nullptr };
}


void NVItemDB::check_randomisation()
{
	if (!s_bHasRandomised)
	{
		DbgWarn("Seeding randomiser...");
		s_RNG.seed(std::random_device()());

		s_bHasRandomised = true;
	}
}

NVItemDB::ItemRow* NVItemDB::random(std::string group_id)
{
	check_randomisation();

	auto group_it = s_ItemDatabase.find(group_id);
	if (group_it == s_ItemDatabase.end())
	{
		return nullptr;
	}

	auto item_it = group_it->second.begin();

	// Advance the iterator.
	std::uniform_int_distribution<std::mt19937::result_type> itemdist(0, group_it->second.size() - 1);
	std::advance(item_it, itemdist(s_RNG));

	return &(item_it->second);
}

NVItemDB::ItemRow* NVItemDB::random_safe_weapon(bool bResultHasNoAmmo)
{
	check_randomisation();

	auto group_it = s_ItemDatabase.find("weapons_safe");
	if (group_it == s_ItemDatabase.end())
	{
		return nullptr;
	}

	item_group_map::iterator result_it;

	while (true)
	{
		item_group_map::iterator item_it = group_it->second.begin();

		// Advance the iterator.
		std::uniform_int_distribution<std::mt19937::result_type> itemdist(0, group_it->second.size() - 1);
		std::advance(item_it, itemdist(s_RNG));

		result_it = item_it;
		if (bResultHasNoAmmo && item_it->second.projectile_id != 0)
		{
			break;
		}
		else if (!bResultHasNoAmmo)
			break;
	}
	return &(result_it->second);
}

NVItemDB::ItemRow* NVItemDB::lookup(std::string form_name)
{
	for (auto group = s_ItemDatabase.begin(); group != s_ItemDatabase.end(); ++group)
	{
		for (auto item_entry = group->second.begin(); item_entry != group->second.end(); ++item_entry)
		{
			if (item_entry->second.form_ref == form_name)
			{
				if (s_pCacheMissLookup != nullptr && item_entry->second.instance_ref == nullptr)
				{
					DbgWarn("NVItemDB cache miss lookup...");
					item_entry->second.instance_ref = s_pCacheMissLookup(item_entry->second.form_id);
				}

				return &(item_entry->second);
			}
		}
	}

	return nullptr;
}