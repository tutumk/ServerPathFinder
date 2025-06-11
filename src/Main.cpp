#include <wx/wx.h>
#include <fstream>
#include <sstream>
#include <string>
#include <Algoritms.h>
 
class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

 
wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame {
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "Path Finder", wxDefaultPosition, wxSize(900, 500)) {
        // Create main panel and sizer
        wxPanel* panel = new wxPanel(this);
        wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

        // File input row
        m_textFileName = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(400, -1));
        wxButton* btnPrintContent = new wxButton(panel, wxID_ANY, "Print file content");
        wxButton* btnProcessContent = new wxButton(panel, wxID_ANY, "Find shortest path");

        wxBoxSizer* fileBox = new wxBoxSizer(wxHORIZONTAL);
        fileBox->Add(m_textFileName, 1, wxALL | wxEXPAND, 5);
        fileBox->Add(btnPrintContent, 0, wxALL, 5);
        fileBox->Add(btnProcessContent, 0, wxALL, 5);

        // Additional processing input fields
        m_textfilesize = new wxTextCtrl(panel, wxID_ANY, "0", wxDefaultPosition, wxSize(100, -1));
        m_textSource = new wxTextCtrl(panel, wxID_ANY, "1", wxDefaultPosition, wxSize(100, -1));
        m_textDestination = new wxTextCtrl(panel, wxID_ANY, "1", wxDefaultPosition, wxSize(100, -1));

        wxStaticText* labelfilesize = new wxStaticText(panel, wxID_ANY, "File size:");
        wxStaticText* labelSource = new wxStaticText(panel, wxID_ANY, "Source:");
        wxStaticText* labelDestination = new wxStaticText(panel, wxID_ANY, "Destination:");

        wxBoxSizer* processBox = new wxBoxSizer(wxHORIZONTAL);
        processBox->Add(labelfilesize, 0, wxALL, 5);
        processBox->Add(m_textfilesize, 1, wxALL | wxEXPAND, 5);
        processBox->Add(labelSource, 0, wxALL, 5);
        processBox->Add(m_textSource, 1, wxALL | wxEXPAND, 5);
        processBox->Add(labelDestination, 0, wxALL, 5);
        processBox->Add(m_textDestination, 1, wxALL | wxEXPAND, 5);

        // Output display
        m_textResult = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 300),
                                      wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);

        vbox->Add(fileBox, 0, wxEXPAND);
        vbox->Add(processBox, 0, wxEXPAND);
        vbox->Add(m_textResult, 1, wxALL | wxEXPAND, 5);

        panel->SetSizer(vbox);

        // Event binding
        Bind(wxEVT_BUTTON, &MyFrame::OnPrintContent, this, btnPrintContent->GetId());
        Bind(wxEVT_BUTTON, &MyFrame::OnProcessContent, this, btnProcessContent->GetId());
    }

private:
    wxTextCtrl* m_textFileName;
    wxTextCtrl* m_textfilesize;
    wxTextCtrl* m_textSource;
    wxTextCtrl* m_textDestination;
    wxTextCtrl* m_textResult;

    bool LoadFileContent(int& N, int& source, int& destination, vector<vector<int>>& edges) {
        readGraphFromFile(m_textFileName->GetValue().ToStdString(),N,source,destination,edges);
        wxString fileName = m_textFileName->GetValue();
        std::ifstream file(fileName.ToStdString());
        if (!file) {
            wxMessageBox("Unable to open file!", "Error", wxOK | wxICON_ERROR);
            return 0;
        }

        std::ostringstream contentStream;
        contentStream << file.rdbuf();
        file.close();
        return 1;
    }

    void OnPrintContent(wxCommandEvent& event) {
        vector<vector<int>> edges;
        int servers, source, destination;
        bool content = LoadFileContent(servers,source,destination,edges);
        if (!content) return;//файл не вдалося завантажити

        if(std::stoi(m_textfilesize->GetValue().ToStdString())<=0){
        m_textResult->SetValue(printGraphUnadjusted(edges));//вивід не перерахованих зв’язків
        } else {
            vector<vector<int>> edges_adjusted;
            edges_adjusted = AdjustBySize(edges, std::stoi(m_textfilesize->GetValue().ToStdString()));
            m_textResult->SetValue(printGraph(edges_adjusted));
            //вивід перерахованих під введений розмір файлу зв’язків
        }
    }

    void OnProcessContent(wxCommandEvent& event) {
    int N;            
    int filesize = ((std::stoi(m_textfilesize->GetValue().ToStdString())>=0) ? std::stoi(m_textfilesize->GetValue().ToStdString()) : 1000);
    vector<vector<int>> edges;
    vector<vector<int>> edges_adjusted;
    int source = std::stoi(m_textSource->GetValue().ToStdString());    
	int destination = std::stoi(m_textDestination->GetValue().ToStdString()); 
    pair<int,vector<int>> result;   
    auto st = high_resolution_clock::now();
    auto en = high_resolution_clock::now();
    
        bool content = LoadFileContent(N,source,destination,edges);
        if (!content) return;
        try {
            edges_adjusted = AdjustBySize(edges, filesize);
            st = high_resolution_clock::now();
            result = Bellman_Ford(N, edges_adjusted, ((std::stoi(m_textSource->GetValue().ToStdString())>=0) ? std::stoi(m_textSource->GetValue().ToStdString()) : source), ((std::stoi(m_textDestination->GetValue().ToStdString())>=0) ? std::stoi(m_textDestination->GetValue().ToStdString()) : destination));
            en = high_resolution_clock::now();
            std::ostringstream resultStream;
            resultStream << "Bellan-Ford:\n"<<printPathAndCost(edges_adjusted, result)<<"Time spent to solve:"<<duration_cast<milliseconds>(en-st).count()<<"ms\n\n";
            m_textResult->SetValue(resultStream.str());
            st = high_resolution_clock::now();
            result = Dijkstra(N, edges_adjusted, ((std::stoi(m_textSource->GetValue().ToStdString())>=0) ? std::stoi(m_textSource->GetValue().ToStdString()) : source), ((std::stoi(m_textDestination->GetValue().ToStdString())>=0) ? std::stoi(m_textDestination->GetValue().ToStdString()) : destination));
            en = high_resolution_clock::now();
            resultStream<<"Dijkstra:\n"<<printPathAndCost(edges_adjusted, result)<<"Time spent to solve: "<<duration_cast<milliseconds>(en-st).count()<<"ms";       
            m_textResult->SetValue(resultStream.str());
            
        } catch (const std::exception& e) {
            wxMessageBox("Something went wrong.", "Error", wxOK | wxICON_ERROR);
        }
    }
};

 
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}
