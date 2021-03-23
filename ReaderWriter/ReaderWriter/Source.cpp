
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <queue>
#include <ctime>

std::vector<std::thread> readerThreads;
std::vector<std::thread> writerThreads;

int bosshealth = 1000;
int numReaders = 0;
int numWriters = 0;
std::mutex readWriteLock;			
std::mutex readerChangeLock;
std::mutex writeChangeLock;

void Reader(int id)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds((rand() % 100) / 10));
		if (numWriters <= 0)
		{
			numReaders = numReaders + 1;

			readWriteLock.lock();
			std::cout <<  "ReaderID:" + std::to_string(id) << ": Boss health: " + std::to_string(bosshealth) << std::endl;
			readWriteLock.unlock();

			numReaders = numReaders - 1;
		}
	}
}

void Writer(int id)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds((rand() % 100) / 10));
		if (numReaders <= 0 && numWriters <= 0)
		{
			numWriters = numWriters + 1;

			readWriteLock.lock();
			std::cout << "                                     Player:" + std::to_string(id) << " attacks" << std::endl;
			bosshealth = bosshealth - 1;
			readWriteLock.unlock();

			numWriters = numWriters - 1;
		}
	}
}

void Reader2(int id)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds((rand() % 100) / 10));
		while (numWriters > 0)
		{
			// Wait for writers to be finished
		}

		readerChangeLock.lock();
		numReaders = numReaders + 1;
		readerChangeLock.unlock();

		readWriteLock.lock();
		std::cout << "ReaderID:" + std::to_string(id) << ": Boss health: " + std::to_string(bosshealth) << std::endl;
		readWriteLock.unlock();

		readerChangeLock.lock();
		numReaders = numReaders - 1;
		readerChangeLock.unlock();
	}
}

void Writer2(int id)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds((rand() % 100) / 10));

		while (numReaders > 0 || numWriters > 0)
		{
			// Wait for no readers or other writers to be working
		}

		writeChangeLock.lock();
		numWriters = numWriters + 1;
		writeChangeLock.unlock();

		readWriteLock.lock();
		std::cout << "                                     Player:" + std::to_string(id) << " attacks" << std::endl;
		bosshealth = bosshealth - 1;
		readWriteLock.unlock();

		writeChangeLock.lock();
		numWriters = numWriters - 1;
		writeChangeLock.unlock();
	}
}

int main()
{
	srand(time(nullptr));

	for (int i = 0; i < 25; i++)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));	// To control random number variance in thread, if they start at the same time then they all get the same random wait time and run together
		std::thread read(Reader2, i);
		readerThreads.push_back(move(read));					// You can't copy Threads so I must move this memory into the vector

		std::thread write(Writer2, i);
		writerThreads.push_back(move(write));
	}

	for (int i = 0; i < 25; i++)
	{
		readerThreads.at(i).join();
		writerThreads.at(i).join();
	}


	std::cin.get();

	return 1;
}