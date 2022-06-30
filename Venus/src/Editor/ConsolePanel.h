#pragma once

#include "spdlog/sinks/base_sink.h"
#include <mutex>

struct ImGuiTextFilter;

namespace Venus {

	enum class Category
	{
		None = 0,
		Info = 1,
		Warning = 2, 
		Error = 3
	};

	class ConsoleMessage
	{
		public:
			ConsoleMessage()
				: m_MessageID(0), m_Message(""), m_Count(0), m_Category(Category::None) {}

			ConsoleMessage(const std::string& message, Category category)
				: m_MessageID(std::hash<std::string>()(message)), m_Message(message), m_Count(1), m_Category(category) {}


			uint32_t GetMessageID() const { return m_MessageID; }
			const std::string& GetMessage() const { return m_Message; }
			uint32_t GetCount() const { return m_Count; }
			Category  GetCategory() const { return m_Category; }

		private:
			uint32_t m_MessageID;
			std::string m_Message;
			uint32_t m_Count;
			Category m_Category;

			friend class ConsolePanel;
	};

	class ConsolePanel
	{
		public:
			ConsolePanel();
			~ConsolePanel();

			void OnImGuiRender(bool& open);
			void OnScenePlay();

		private:
			void RenderMenu();
			void RenderConsole(ImGuiTextFilter filter);

			//--
			static void PushMessage(const ConsoleMessage& message);

		private:

			// Options
			bool m_ClearOnPlay = false;
			int m_SelectedFilter = 0;

			static constexpr uint32_t s_MessageBufferCapacity = 300;
			std::array<ConsoleMessage, s_MessageBufferCapacity> m_MessageBuffer;
			uint32_t m_MessageBufferIndex = 0;

			friend class ConsoleSink;
	};

	class ConsoleSink : public spdlog::sinks::base_sink<std::mutex>
	{
		public:
			explicit ConsoleSink(uint32_t bufferCapacity)
				: m_BufferCapacity(bufferCapacity), m_MessageBuffer(bufferCapacity) {}

			virtual ~ConsoleSink() = default;

			ConsoleSink(const ConsoleSink& other) = delete;
			ConsoleSink& operator=(const ConsoleSink& other) = delete;

		protected:
			void sink_it_(const spdlog::details::log_msg& msg) override
			{
				spdlog::memory_buf_t formatted;
				spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
				m_MessageBuffer[m_MessageCount++] = ConsoleMessage(fmt::to_string(formatted), GetMessageCategory(msg.level));

				if (m_MessageCount == m_BufferCapacity)
					flush_();
			}

			void flush_() override
			{
				for (const auto& message : m_MessageBuffer)
				{
					if (message.GetCategory() == Category::None)
						continue;

					ConsolePanel::PushMessage(message);
				}

				m_MessageCount = 0;
			}

		private:
			static Category GetMessageCategory(spdlog::level::level_enum level)
			{
				switch (level)
				{
					case spdlog::level::trace:
					case spdlog::level::debug:
					case spdlog::level::info:		return Category::Info;
					case spdlog::level::warn:		return Category::Warning;
					case spdlog::level::err:
					case spdlog::level::critical:	return Category::Error;
				}

				return Category::None;
			}

		private:
			uint32_t m_BufferCapacity;
			std::vector<ConsoleMessage> m_MessageBuffer;
			uint32_t m_MessageCount = 0;
	};
}

