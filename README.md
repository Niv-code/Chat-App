# Chat App
#AI Desktop Chat 

A full stack desktop chat application that connects a modern **C# WPF client** to Google's **Gemini AI** via a custom **C++ TCP Server**. 

Instead of directly querying the API from the frontend, this project uses a C++ backend server to handle all internet communications, build the JSON payloads, and manage sockets, making it a great demonstration of cross-language communication, network programming, and API integration.

## Features
* **Modern UI:** A sleek, WPF client featuring WhatsApp style chat bubbles, auto scrolling, and responsive design.
* **AI Powered:** Integrated with Google's `gemini-2.5-flash` model for fast and smart responses.
* **Custom C++ Server:** Uses Windows Sockets (`Winsock2`) to accept TCP client connections and multithreading to handle requests without freezing.
* **Zero External HTTP Libraries:** The C++ server cleverly utilizes the native Windows `curl` command via `_popen` to communicate with the Gemini REST API, avoiding the complex setup of `libcurl`.

## 🛠️ Tech Stack
* **Frontend:** C# / WPF (Windows Presentation Foundation), XAML, MVVM-inspired Data Binding.
* **Backend:** Standard C++11 (or higher), Winsock2, `<thread>`.
* **API:** Google Gemini API (REST / JSON).
