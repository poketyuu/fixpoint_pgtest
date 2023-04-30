#include <bits/stdc++.h>
using namespace std;
tm GetTMFromString(string s)
{
    // YYYYMMDDhhmmss形式の文字列をtm型に変換
    smatch results;
    string pattern = R"((\d{4})(\d{2})(\d{2})(\d{2})(\d{2})(\d{2}))";
    if (regex_match(s, results, regex(pattern)))
    {
        try
        {
            int year = stoi(results[1].str());
            int month = stoi(results[2].str());
            int day = stoi(results[3].str());
            int hour = stoi(results[4].str());
            int minute = stoi(results[5].str());
            int second = stoi(results[6].str());
            return tm{second, minute, hour, day, month, year};
        }
        catch (exception &e)
        {
        }
    }
    return tm{0, 0, 0, 0, 0, 0};
}
class Address
{
public:
    int prefix;
    bitset<32> ipv4address;
    Address(string serveraddress)
    {
        stringstream ss(serveraddress);
        string s;
        int i = 0;
        while (getline(ss, s, '/'))
        {
            if (i == 0)
            {
                stringstream sstream(s);
                string numstring;
                vector<int> addressvec(0);
                while (getline(sstream, numstring, '.'))
                {
                    int n = 0;
                    try
                    {
                        n = stoi(numstring);
                    }
                    catch (exception &e)
                    {
                    }
                    addressvec.push_back(n);
                }
                if (addressvec.size() == 4)
                {
                    bitset<32> b((addressvec[0] << 24) + (addressvec[1] << 16) + (addressvec[2] << 8) + addressvec[3]);
                    ipv4address = b;
                }
            }
            else if (i == 1)
            {
                prefix = stoi(s);
            }
            i++;
        }
    }
    bitset<32> subnet()
    {
        return (ipv4address >> (32 - prefix));
    }
    string GetString()
    {
        bitset<32> b(255);
        long long l1 = ((ipv4address & (b << 24)) >> 24).to_ullong();
        long long l2 = ((ipv4address & (b << 16)) >> 16).to_ullong();
        long long l3 = ((ipv4address & (b << 8)) >> 8).to_ullong();
        long long l4 = (ipv4address & b).to_ullong();
        return to_string(l1) + "." + to_string(l2) + "." + to_string(l3) + "." + to_string(l4) + "/" + to_string(prefix);
    }
    bool operator<(const Address &value) const
    {
        if (ipv4address.to_ullong() < value.ipv4address.to_ullong())
            return true;
        return false;
    }
};
class AccessLog
{
public:
    bool success = true;
    int ping;
    tm accesstime;
    int misscount;
    AccessLog(string timedata, string pingdata)
    {
        accesstime = GetTMFromString(timedata);
        if (pingdata == "-")
        {
            success = false;
            ping = 0;
        }
        else
        {
            try
            {
                ping = stoi(pingdata);
            }
            catch (exception &e)
            {
                success = false;
                ping = 0;
            }
        }
        misscount = 1;
    }
    void AddMissCount(){
        misscount++;
        return;
    }
};
int main(int args, char *argv[])
{
    string logfile;
    int N = 0;
    // 本番環境用
    
    logfile = argv[1];
    try{
        N = stoi(argv[2]);
    }catch(exception& e){

    }
    
    //cin >> logfile;
    //cin >> N;
    vector<pair<string, double>> errlog;
    ifstream ifs(logfile);
    // ファイルが存在しているかを判定する
    if (!ifs)
    {
        cout << "該当するログファイルが存在しません。" << endl;
        return 0;
    }
    map<Address, AccessLog> monitor;
    string line;
    while (getline(ifs, line))
    {
        // ファイルの内容を1行ずつ読む
        stringstream ss(line);
        string s;
        vector<string> vs;
        while (getline(ss, s, ','))
        {
            vs.push_back(s);
        }
        Address ad(vs[1]);
        AccessLog alog(vs[0], vs[2]);
        auto result = monitor.insert(make_pair(ad, alog));
        if (!result.second)
        {
            AccessLog& prevlog = result.first->second;
            if (prevlog.success)
            {
                prevlog = alog;
            }
            else
            {
                if (alog.success)
                {
                    if (prevlog.misscount >= N)
                    {
                        time_t prev = mktime(&prevlog.accesstime);
                        time_t cur = mktime(&alog.accesstime);
                        double timeout = difftime(cur, prev) + alog.ping / 1000.0;
                        errlog.push_back(make_pair(ad.GetString(), timeout));
                    }
                    prevlog = alog;
                }else{
                    prevlog.AddMissCount();
                }
            }
        }
    }
    // 書き込み処理
    ofstream ofs("serverlog.csv");
    ofs << "Server Address , Timeout term" << endl;
    for (int i = 0; i < errlog.size(); i++)
    {
        ofs << errlog[i].first << "," << errlog[i].second << endl;
    }
    ofs.close();
    return 0;
}