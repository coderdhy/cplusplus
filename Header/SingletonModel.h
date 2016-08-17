#ifndef __SINGLETON_MODEL_H__
#define __SINGLETON_MODEL_H__

namespace dhymodel
{
	template<class T>
	class CSingletonModel
	{
	public:
		virtual ~CSingletonModel() {};
		T* Instance()
		{
			static T s_single;
			return &s_single;
		}

	private:
		CSingletonModel();
		CSingletonModel(const CSingletonModel& model) {};
		CSingletonModel& operator=(CSingletonModel& model) { };
	};
}

#endif