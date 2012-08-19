/*
 * Appcelerator Kroll - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _KR_PROFILED_BOUND_OBJECT_H_
#define _KR_PROFILED_BOUND_OBJECT_H_
#include <Poco/FileStream.h>
#include <Poco/Mutex.h>

namespace tide
{
	/**
	 * The ProfiledBoundObject is a wrapped Object that does profiling on a 
	 * wrapped Object
	 */
	class TIDE_API ProfiledBoundObject : public Object
	{
		public:
		ProfiledBoundObject(KObjectRef delegate);
		virtual ~ProfiledBoundObject();
		static void SetStream(Poco::FileOutputStream*);

		public:
		// @see Object::Set
		virtual void Set(const char *name, KValueRef value);
		// @see Object::Get
		virtual KValueRef Get(const char *name);
		// @see Object::GetPropertyNames
		virtual SharedStringList GetPropertyNames();
		// @see Object::DisplayString
		virtual SharedString DisplayString(int levels=3);
		// @see Object::Equals
		virtual bool Equals(KObjectRef other);

		bool HasProperty(const char* name);

		/**
		 * @return the delegate of this profiled bound object
		 */
		KObjectRef GetDelegate() { return delegate; }
		virtual void duplicate()
		{
			++count;
		}

		virtual void release()
		{
			int value = --count;
			if (value <= 0) {
				delete this;
			}
		}

		virtual int referenceCount() const
		{
			return count.value();
		}

	protected:
		KObjectRef delegate;
		KValueRef Wrap(KValueRef value, std::string type);
		std::string GetSubType(std::string name);
		void Log(const char* eventType, std::string& name, Poco::Timestamp::TimeDiff);
		static bool AlreadyWrapped(KValueRef);
		static Poco::FileOutputStream *stream;
		static Poco::Mutex logMutex;
		Poco::AtomicCounter count;
	};
}

#endif