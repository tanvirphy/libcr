/** @file Semaphore.hpp
	Contains the thread-safe semaphore type. */
#ifndef __libcr_mt_semaphore_hpp_defined
#define __libcr_mt_semaphore_hpp_defined

#include <cstddef>

#include "detail/SoftMutex.hpp"
#include "../util/Atomic.hpp"
#include "../sync/Block.hpp"
#include "ConditionVariable.hpp"


namespace cr
{
	class Coroutine;
}

namespace cr::mt
{
	template<class ConditionVariable>
	/** POD semaphore type.
	@tparam ConditionVariable:
		The condition variable type to use. */
	class PODSemaphoreBase
	{
		/** Soft mutex protecting the semaphore. */
		detail::PODSoftMutex m_mutex;
		/** The condition variable. */
		ConditionVariable m_cv;
		/** The semaphore's count. */
		util::Atomic<std::size_t> m_count;
		/** The count of currently registering coroutines. */
		util::Atomic<std::size_t> m_registering;
	public:
		/** Initialises the semaphore. */
		void initialise(
			std::size_t count = 0);
		/** Notifies the semaphore. */
		void notify();

		/** Helper type for waiting for a semaphore using `#CR_AWAIT`. */
		class WaitCall
		{
			/** The semaphore to wait for. */
			PODSemaphoreBase<ConditionVariable> &m_semaphore;
		public:
			/** Initialises the wait call.
			@param[in] semaphore:
				The semaphore to wait for. */
			constexpr WaitCall(
				PODSemaphoreBase<ConditionVariable> * semaphore);

			/** Waits for the semaphore.
			@param[in] coroutine:
				The coroutine to wait for the semaphore.
			@return
				Whether the operation is blocking. */
			[[nodiscard]] sync::mayblock libcr_wait(
				Coroutine * coroutine);
		};

		/** Waits for the semaphore.
			To be used with `#CR_AWAIT`. */
		[[nodiscard]] constexpr WaitCall wait();
	private:
		/** Tries to wait for the semaphore. */
		bool try_wait();
	};

	template<class ConditionVariable>
	/** Non-POD semaphore type.
	@tparam ConditionVariable
		The condition variable type to use. */
	class SemaphoreBase : public PODSemaphoreBase<ConditionVariable>
	{
		using PODSemaphoreBase<ConditionVariable>::initialise;
	public:
		/** Initialises the semaphore. */
		inline SemaphoreBase();
		/** Initialises the semaphore.
		@param[in] count:
			The semaphore's initial count. */
		explicit inline SemaphoreBase(
			std::size_t count);
	};

	typedef PODSemaphoreBase<PODConditionVariable> PODSemaphore;
	typedef PODSemaphoreBase<PODFIFOConditionVariable> PODFIFOSemaphore;
	typedef SemaphoreBase<PODConditionVariable> Semaphore;
	typedef SemaphoreBase<PODFIFOConditionVariable> FIFOSemaphore;
}

#include "Semaphore.inl"

#endif