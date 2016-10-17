#ifndef _NOTIFIER_CLASS_H_
#define _NOTIFIER_CLASS_H_

#include <vector>
#include <algorithm>

#include "listenerClass.h"

template <class T, class A>
class Notifier
{
	public:
		Notifier(){};
		//Notifier(const Notifier<T, A> &);
		virtual ~Notifier(){};

		bool addListener(Listener<T, A> &listener)
		{
			std::vector<Listener<T, A> *>::const_iterator iterator = std::find(listeners_.begin(), listeners_.end(), &listener);
			if (iterator == listeners_.end()) {
				listeners_.push_back(&listener);
				return true;
			}

			return false;
		};

		bool removeListener(Listener<T, A> &listener)
		{
			std::vector<Listener<T,A> *>::const_iterator iterator = find(listeners_.begin(), listeners_.end(), &listener);
			if (iterator == listeners_.end())
				return false;
			else
				listeners_.erase(remove(listeners_.begin(), listeners_.end(), &listener));
			return true;
		};

		bool notifyListeners(A arg)
		{
			std::vector<Listener<T,A> *>::iterator temp;
			for(temp = listeners_.begin(); temp != listeners_.end(); temp++)
			{
				/*std::stringstream listenersNumber;
				listenersNumber << "Listeners: " << listeners_.size();
				MessageBoxA(NULL, listenersNumber.str().c_str(), "Notifier - Fruit - Error", MB_ICONERROR | MB_OK);*/
				(*temp)->notify(static_cast<T *>(this), arg);
			}

			return (listeners_.size() > 0);
		};

	private: 
		std::vector<Listener<T, A> *> listeners_;

};

#endif //_NOTIFIER_CLASS_H_
