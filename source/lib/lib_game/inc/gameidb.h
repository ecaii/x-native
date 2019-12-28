#pragma once
#include <cinttypes>
#include <string>

class TESForm;

struct ItemRow
{
	std::uint32_t form_id;
	std::string   form_ref;
	std::string   display_name;

	TESForm*      instance_ref;
	std::uint32_t projectile_id;
};

// Must be included before item row is declared.
#include "gameidbprefix.h"

class NVItemDB
{
public:
	typedef TESForm*(*_remote_lookup)(unsigned long id);
	using ItemRow = ItemRow;
	using ItemRow_Verbose = std::pair<std::string, ItemRow*>;

	//-----------------------------------------------
	// Helper functions that convert standard form
	// tags to their associated ItemRow entries.
	//-----------------------------------------------
	static ItemRow* lookup(std::uint32_t form_id);
	static ItemRow* lookup(std::string   form_name);

	static ItemRow_Verbose lookup_verbose(std::uint32_t form_id);
	static ItemRow_Verbose lookup_verbose(std::string   form_name);
	static ItemRow* first_entry_in_ammo_list(std::uint32_t ammo_list_id);

	static item_group_map* lookup_group(std::string form_name);

	//-----------------------------------------------
	// Returns an ItemRow object randomly from the 
	// passed group ID. Returns nullptr if the 
	// group doesn't exist, or is empty.
	//
	// Some items returned may be broken or meant
	// for Bethesda development. Use random_safe 
	// to get weapons confirmed to be safe.
	//-----------------------------------------------
	static ItemRow* random(std::string group_id);

	//-----------------------------------------------
	// Returns an ItemRow object which is a weapon
	// randomly from the passed group ID. 
	//-----------------------------------------------
	static ItemRow* random_safe_weapon(bool bResultHasNoAmmo = false);

	//-----------------------------------------------
	// Processes all records and cache's their 
	// FormLookupId results. Should only be called
	// on the client.
	//-----------------------------------------------
	static void Initialize(_remote_lookup lookup_fn);

private:
	static void check_randomisation();
};