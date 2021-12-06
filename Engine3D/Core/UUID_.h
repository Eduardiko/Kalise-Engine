#pragma once

#include <xhash>


class UUID_
{
public:
	UUID_();
	UUID_(uint64_t uuid);
	UUID_(const UUID_&) = default;

	operator uint64_t() const { return m_UUID; }

private:
	uint64_t m_UUID;
};


namespace std {
	template<>
	struct hash<UUID_>
	{
		std::size_t operator()(const UUID_& uuid) const {
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}