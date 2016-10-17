#ifndef _LISTENER_CLASS_H_
#define _LISTENER_CLASS_H_

template <class T, class A>
class Notifier;

template <class T, class A>
class Listener
{
	public:
		Listener(){};
		//Listener(const Listener<T, A> &);
		virtual ~Listener(){};

		virtual void notify(T* notifier, A arg) = 0;
};

#endif //_LISTENER_CLASS_H_
