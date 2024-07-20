#pragma once

class RunCmdAsync : public SimpleThreadTask
{
public:
	RunCmdAsync(CWindow wnd, std::wstring_view app, std::wstring_view params, uintptr_t task_id) : m_wnd(wnd), m_app(app), m_params(params), m_task_id(task_id) {}

	void run() final
	{
		STARTUPINFOW si{};
		si.cb = sizeof(si);
		wil::unique_process_information pi;

		if (CreateProcessW(m_app.data(), m_params.data(), nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
		{
			WaitForSingleObject(pi.hProcess, INFINITE);
			m_wnd.SendMessageW(std::to_underlying(CallbackID::on_run_cmd_async_done), m_task_id);
		}
	}

private:
	CWindow m_wnd;
	std::wstring m_app, m_params;
	uintptr_t m_task_id{};
};
