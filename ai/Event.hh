#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <unordered_map>
#include <iostream>

template <typename... TArgs>
class Event
{
public:
	using Handler = void (*)(TArgs...);
	template <class TOwner>
	using MemberHandler = void (TOwner::*)(TArgs...);

	Event() = default;
	virtual ~Event() = default;

	template <class TOwner>
	void Add(TOwner* owner, MemberHandler<TOwner> handler)
	{
		const auto key(MakeKey(owner, &handler));
		if (m_entryMappings.find(key) != m_entryMappings.end())
			return;

		const auto proxy([=](TArgs&&... args) { (owner->*handler)(args...); });
		const auto id(++m_nextHandlerId);
		const auto& entry(m_handlers.emplace_back(id, proxy));
		m_entryMappings.emplace(key, id);
	}

	void Add(Handler handler)
	{
		const auto key(MakeKey(nullptr, &handler));
		if (m_entryMappings.find(key) != m_entryMappings.end())
			return;

		const auto id(++m_nextHandlerId);
		const auto& entry(m_handlers.emplace_back(id, handler));
		m_entryMappings.emplace(key, id);
	}

	template <class TOwner>
	bool Remove(TOwner* owner, MemberHandler<TOwner> handler)
	{
		const auto key(MakeKey(owner, &handler));
		const auto found(m_entryMappings.find(key));
		if (found == m_entryMappings.end())
			return false;

		const auto entryFind(std::find(m_handlers.begin(), m_handlers.end(), found->second));
		m_handlers.erase(entryFind); //preserve order
		m_entryMappings.erase(found);
		return true;
	}

	bool Remove(Handler handler)
	{
		const auto key(MakeKey(nullptr, &handler));
		const auto found(m_entryMappings.find(key));
		if (found == m_entryMappings.end())
			return false;
		
		const auto entryFind(std::find(m_handlers.begin(), m_handlers.end(), found->second));
		m_handlers.erase(entryFind); //preserve order
		m_entryMappings.erase(found);
		return true;
	}

	void Invoke(TArgs... args) const
	{
		for (const auto& handler : m_handlers)
			handler.handler(std::forward<TArgs>(args)...); // gross
	}

	void Clear()
	{
		m_handlers.clear();
		m_entryMappings.clear();
	}

private:
	struct HandlerEntry
	{
		size_t id;
		std::function<void(TArgs...)> handler;

		bool operator==(size_t otherId)
		{
			return id == otherId;
		}
	};

	size_t MakeKey(void* owner, void* handler)
	{
		// cantor pairing
		const auto sum((size_t)owner + (size_t)handler);
		return (sum / 2) * (sum) + (size_t)handler;
	}

	std::vector<HandlerEntry> m_handlers;
	std::unordered_map<size_t, size_t> m_entryMappings;
	size_t m_nextHandlerId{ 0 };
};