#include <wx/wx.h>
#include <sstream>
#include <chrono>

#include <ultra64.h>
#include <rom.h>
#include <pif_rom.h>
#include <memory.h>
#include <r4300.h>
#include <rsp.h>

ultra64::ROM *rom;
ultra64::PIF_ROM *pif_rom;
ultra64::MMU *mmu = new ultra64::MMU();
ultra64::r4300 *cpu = new ultra64::r4300();
ultra64::rsp *rsp = new ultra64::rsp();

void open_registers();

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_MENU(ID_open_pif_rom, MainWindow::OnOpenPIFROM)
    EVT_MENU(ID_open_rom,   MainWindow::OnOpenROM)
    EVT_MENU(ID_debug_pif_rom, MainWindow::OnDebugPIFROM)
    EVT_MENU(ID_debug_rom, MainWindow::OnDebugROM)
    EVT_MENU(ID_debug_registers, MainWindow::OnDebugRegisters)
    EVT_MENU(wxID_EXIT,  MainWindow::OnExit)
    EVT_MENU(wxID_ABOUT, MainWindow::OnAbout)
wxEND_EVENT_TABLE()

wxBEGIN_EVENT_TABLE(DebuggerWindow, wxFrame)
    EVT_BUTTON(BUTTON_cpu_step, DebuggerWindow::OnCPUStep)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(wxUltra64);

bool wxUltra64::OnInit()
{
    frame = new MainWindow("Ultra 64", wxPoint(20, 20), wxSize(640, 480));
    frame->Show(true);
    return true;
}

DebuggerWindow::DebuggerWindow(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(parent, wxID_ANY, title, pos, size)
{
}

DebuggerWindow::~DebuggerWindow()
{
    wxGetApp().debugger = NULL;
}

void DebuggerWindow::OnCPUStep(wxCommandEvent& event)
{
    try
    {
        cpu->step();
    }
    catch(std::string e)
    {
        wxGetApp().frame->SetStatusText(e, 0);
    }

    if(wxGetApp().registers != NULL) open_registers();
}

RegistersWindow::RegistersWindow(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(parent, wxID_ANY, title, pos, size)
{
}

RegistersWindow::~RegistersWindow()
{
    wxGetApp().registers = NULL;
}

MainWindow::MainWindow(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    this->SetBackgroundColour(wxColour(*wxBLACK));

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_open_pif_rom, "Load &PIF ROM...","");
    menuFile->Append(ID_open_rom, "&Open ROM...\tCtrl-H",
                     "");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenu *menuDebug = new wxMenu;
    menuDebug->Append(ID_debug_pif_rom, "View &PIF ROM", "");
    menuDebug->Append(ID_debug_rom, "View &ROM", "");
    menuDebug->Append(ID_debug_registers, "View R&egisters", "");

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuDebug, "&Debug");
    menuBar->Append(menuHelp, "&Help");
    SetMenuBar(menuBar);

    CreateStatusBar(1);
    SetStatusText(wxT("Ready"), 0);
}

void MainWindow::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MainWindow::OnAbout(wxCommandEvent& event)
{
    wxMessageBox( "This is a wxWidgets' Hello world sample",
                  "About Hello World", wxOK | wxICON_INFORMATION );
}

void MainWindow::OnOpenPIFROM(wxCommandEvent& event)
{
    wxFileDialog rom_dialog(this, _("Open PIF ROM file"), "", "",
                                "ROM files (*.bin)|*.bin|All files (*.*)|*.*",
                                wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if(wxID_CANCEL == rom_dialog.ShowModal()) return;

    std::string filename = rom_dialog.GetPath().ToStdString();
    if(!filename.size()) return;

    try
    {
        pif_rom = new ultra64::PIF_ROM(filename);
    }
    catch(std::string e)
    {
        std::cout << e << std::endl;
        return;
    }

    uint32_t count = 0;
    uint32_t *p = (uint32_t *)pif_rom->get_pointer();

    while(count < 0x7C0)
    {
        mmu->write_word(0x1FC00000 + count, p[count / 4]);
        count += 4;
    }
}

void MainWindow::OnOpenROM(wxCommandEvent& event)
{
    wxFileDialog rom_dialog(this, _("Open N64 ROM file"), "", "",
                                "N64 ROM files (*.n64;*.v64;*.z64;*.bin)|*.n64;*.v64;*.bin|All files (*.*)|*.*",
                                wxFD_OPEN|wxFD_FILE_MUST_EXIST);

    if(wxID_CANCEL == rom_dialog.ShowModal()) return;

    std::string filename = rom_dialog.GetPath().ToStdString();
    if(!filename.size()) return;

    try
    {
        rom = new ultra64::ROM(filename);
    }
    catch(std::string e)
    {
        std::cout << e << std::endl;
        return;
    }

    mmu->attach_rom(rom->get_pointer(), rom->get_size());

    std::stringstream ss;

    ultra64::ROM_header header = rom->get_header();
    ss << "Ultra 64 - " << header.name;

    wxTopLevelWindow::SetTitle(ss.str());
}

void open_debugger()
{
    if(!wxGetApp().debugger) 
    {
        wxGetApp().debugger = new DebuggerWindow(wxGetApp().frame, 
                              "Ultra 64 Debugger", wxPoint(150, 50), wxSize(700, 480));
    }
    wxGetApp().debugger->SetBackgroundColour(wxColour(*wxWHITE));
    wxGetApp().debugger->Show(true);

    wxFont font(wxFontInfo(14).Bold());
    wxStaticText *label_memory_address = new wxStaticText(wxGetApp().debugger, 
                            wxID_ANY, wxT("Address      Data"), wxPoint(20, 5));
    label_memory_address->SetFont(font);

    wxFont mem_font(wxFontInfo(12).FaceName("Courier"));
    wxGetApp().debugger->memory_listbox = new wxListBox(wxGetApp().debugger, ID_MEMORY_LISTBOX, 
      wxPoint(20, 30), wxSize(550, 380)); 
    wxGetApp().debugger->memory_listbox->SetFont(mem_font);

    wxGetApp().debugger->cpu_step = new wxButton(wxGetApp().debugger, BUTTON_cpu_step, _T("Step"), 
                                                 wxPoint(580, 30), wxDefaultSize, 0);
}

void open_registers()
{
    if(!wxGetApp().registers)
    {
        wxGetApp().registers = new RegistersWindow(wxGetApp().frame,
                               "Registers", wxPoint(200, 50), wxSize(820, 450));
    }
    wxGetApp().registers->SetBackgroundColour(wxColour(*wxWHITE));
    wxGetApp().registers->Show(true);

    wxFont font(wxFontInfo(14).Bold());
    wxStaticText *label_registers = new wxStaticText(wxGetApp().registers,
                            wxID_ANY, wxT("Registers"), wxPoint(20, 5));
    label_registers->SetFont(font);

    wxFont reg_font(wxFontInfo(12).FaceName("Courier"));
    wxGetApp().registers->registers_listbox = new wxListBox(wxGetApp().registers, ID_REGISTERS_LISTBOX,
      wxPoint(20, 30), wxSize(780, 380));
    wxGetApp().registers->registers_listbox->SetFont(reg_font);

    char temp[1024];

    uint32_t count = 0;
    while(count < 32)
    {
        sprintf(temp, "GPR[%.2d]=0x%.8llX  GPR[%.2d]=0x%.8llX  CP0[%.2d]=0x%.8llX  CP0[%.2d]=0x%.8llX",
                count, cpu->get_GPR(count), count + 1, cpu->get_GPR(count + 1),
                count, cpu->get_CP0(count), count + 1, cpu->get_CP0(count + 1));
        wxGetApp().registers->registers_listbox->Append(temp);
        count += 2;
    }

    sprintf(temp, "PC = 0x%.8X", cpu->get_PC());
    wxGetApp().registers->registers_listbox->Append(temp);
}

void MainWindow::OnDebugRegisters(wxCommandEvent &event)
{
    open_registers();
}

void MainWindow::OnDebugPIFROM(wxCommandEvent& event)
{
    open_debugger();
    wxGetApp().debugger->view(0x1FC00000, 0x1FC07BF);
}

void MainWindow::OnDebugROM(wxCommandEvent& event)
{
    open_debugger();
    wxGetApp().debugger->view(0x10000000, 0x10008000);
}

void DebuggerWindow::view(uint32_t start_addr, uint32_t end_addr)
{
    char message[1024];
    std::string msg;
    uint32_t value, addr = start_addr;
    ultra64::opcode_t *op;

#if 0
    auto start_time = std::chrono::high_resolution_clock::now();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto t = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
#endif

    while(addr < end_addr)
    {
        try
        {
            value = mmu->read_word(addr);
            sprintf(message, "%.4X %.4X   %.2X %.2X %.2X %.2X   ", (addr >> 16), (addr & 0xFFFF), 
                   value >> 24, (value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
            op = new ultra64::opcode_t(value); 
            msg = message + op->to_string();
            delete op;
            memory_listbox->Append(msg.c_str());
            addr += 4;
        }
        catch(std::string e)
        {
            memory_listbox->Append(e.c_str());
            return;
        }
    }
}
