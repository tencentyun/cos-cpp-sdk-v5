/*! \file
* \brief Thread pool worker.
*
* The worker thread instance is attached to a pool 
* and executes tasks of this pool. 
*
* Copyright (c) 2005-2007 Philipp Henkel
*
* Use, modification, and distribution are  subject to the
* Boost Software License, Version 1.0. (See accompanying  file
* LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*
* http://threadpool.sourceforge.net
*
*/

#ifndef THREADPOOL_DETAIL_WORKER_THREAD_HPP_INCLUDED
#define THREADPOOL_DETAIL_WORKER_THREAD_HPP_INCLUDED


#include "scope_guard.hpp"

#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/exceptions.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>


namespace boost { namespace threadpool { namespace detail 
{

  /*! \brief Thread pool worker. 
  *
  * A worker_thread represents a thread of execution. The worker is attached to a 
  * thread pool and processes tasks of that pool. The lifetime of the worker and its 
  * internal boost::thread is managed automatically.
  *
  * This class is a helper class and cannot be constructed or accessed directly.
  * 
  * \see pool_core
  */ 
  template <typename Pool>
  class worker_thread
  : public enable_shared_from_this< worker_thread<Pool> > 
  , private noncopyable
  {
  public:
    typedef Pool pool_type;         	   //!< Indicates the pool's type.

  private:
    shared_ptr<pool_type>      m_pool;     //!< Pointer to the pool which created the worker.
    shared_ptr<boost::thread>  m_thread;   //!< Pointer to the thread which executes the run loop.

	boost::barrier m_start_barrier;        //!< Barrier used to synch the startup of the thread

    
    /*! Constructs a new worker. 
    * \param pool Pointer to it's parent pool.
    * \see function create_and_attach
    */
    worker_thread(shared_ptr<pool_type> const & pool)
    : m_pool(pool),
	m_start_barrier( 2 )
    {
      assert(pool);
    }

	
	/*! Notifies that an exception occurred in the run loop.
	*/
	void died_unexpectedly()
	{
		m_pool->worker_died_unexpectedly(this->shared_from_this());
	}


  public:
	  /*! Executes pool's tasks sequentially.
	  */
	  void run()
	  { 
		  //scope_guard notify_exception(bind(&worker_thread::died_unexpectedly, this));
		  //self holder, used to prevent deletion of the worker
		  shared_ptr<worker_thread> self(this->shared_from_this());

		  //while(m_pool->execute_task()) {}
		  //signal the create_and_attach method that thread have started and shared pointer to worker
		   //has been acquired
			m_start_barrier.wait();

		  /*notify_exception.disable();
		  m_pool->worker_destructed(this->shared_from_this());
	  }*/

			scope_guard notify_exception(bind(&worker_thread::died_unexpectedly, this));

			while(m_pool->execute_task()) {}

			notify_exception.disable();

			//After this call there will be one more reference to the worker in m_terminated_workers vector
			//so the instance of worker will not be destructed when method returns
			m_pool->worker_destructed(this->shared_from_this());
		}

	  /*! Joins the worker's thread.
	  */
	  void join()
	  {
		  m_thread->join();
	  }


	  /*! Constructs a new worker thread and attaches it to the pool.
	  * \param pool Pointer to the pool.
	  */
	  static void create_and_attach(shared_ptr<pool_type> const & pool)
	  {
		  /*shared_ptr<worker_thread> worker(new worker_thread(pool));
		  if(worker)
		  {
			  worker->m_thread.reset(new boost::thread(bind(&worker_thread::run, worker)));
		  }
	  }*/
		  shared_ptr<worker_thread> worker(new worker_thread(pool));
		   if(worker)
			  {
				 //Use the real pointer to the worker to prevent holding of the circular references
				//to the worker in thread object
					 worker->m_thread.reset(new boost::thread(bind(&worker_thread::run, &*worker)));

				  //Wait until shared pointer to this is acquired in run method
					worker->m_start_barrier.wait();
				}
		 }

  };


} } } // namespace boost::threadpool::detail

#endif // THREADPOOL_DETAIL_WORKER_THREAD_HPP_INCLUDED

