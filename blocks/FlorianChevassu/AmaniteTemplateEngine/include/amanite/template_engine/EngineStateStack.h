#pragma once

namespace amanite{
	namespace template_engine{

		class EngineStateStack {
			struct EngineState{
				bool skipText = false;
				bool verbatim = false;
				bool escape = false;

				int contextOffset = 0;
			};
			std::stack <EngineState> m_stack;

		public:
			EngineStateStack(){
				//create the initial stack state.
				m_stack.push(EngineState());
			}


			/**
			* Add a state to the engine state stack, and reset tags that are not "heritable".//TODO : heritable ???
			*/
			void pushState() {
				m_stack.push(m_stack.top());

				//erase all tags that are not heritable
				resetTag(CONTEXT_OFFSET);
			}

			/**
			* Pop a state from the engine state stack.
			*/
			void popState() {
				m_stack.pop();
			}

			EngineState& getCurrentState(){
				return m_stack.top();
			}



			//tags
			enum Tag {
				SKIP_TEXT,
				VERBATIM,
				CONTEXT_OFFSET,
				ESCAPE
			};

			static Tag getTag(const std::string& tagStr) {
				Tag res;
				if(tagStr.compare("skipText") == 0)
					res = SKIP_TEXT;
				else if(tagStr.compare("verbatim") == 0)
					res = SKIP_TEXT;
				else if(tagStr.compare("contextOffset") == 0)
					res = CONTEXT_OFFSET;
				else if(tagStr.compare("escape") == 0)
					res = ESCAPE;
				else
					throw std::runtime_error("BAD TAG" + tagStr);

				return res;
			}


			/**
			* Apply the boolean tag \var tag to the state. If negate is set to true, the tag will have the inverse behavior.
			*/
			void applyTag(Tag tag, bool negate = false) {
				switch(tag) {
					case SKIP_TEXT:
						getCurrentState().skipText = !negate;
						break;
					case VERBATIM:
						getCurrentState().verbatim = !negate;
						break;
					case ESCAPE:
						getCurrentState().escape = !negate;
						break;
				}
			}

			/**
			* Apply the value tag \var tag to the state with value \var value.
			*/
			void applyTag(Tag tag, int value) {
				switch(tag) {
					case CONTEXT_OFFSET:
						getCurrentState().contextOffset = value;
						break;
				}
			}

			/**
			* Reset a particular tag.
			*/
			void resetTag(Tag tag) {
				switch(tag) {
					case CONTEXT_OFFSET:
						getCurrentState().contextOffset = 0;
						break;
				}
			}


			/**
			* Apply a set of tags to the current engine state.
			*/
			void applyTags(const std::deque <std::string>& tags) {
				std::for_each(std::begin(tags), std::end(tags), [this](const std::string& tag) {
					std::size_t pos = tag.find('=');
					if(pos != std::string::npos) {
						std::string tagSpelling = tag.substr(0, pos);
						int tagValue = std::stoi(tag.substr(pos + 1));
						applyTag(EngineStateStack::getTag(tagSpelling), tagValue);
					} else {
						if(tag[0] == '!') {
							applyTag(EngineStateStack::getTag(tag.substr(1)), true);
						} else {
							applyTag(EngineStateStack::getTag(tag));
						}
					}
				});
			}
		};
	}
}