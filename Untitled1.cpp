#include<stdio.h>
int main()
{
    int n,bt[3]={8,4,1},wt[3],tat[3],avwt=0,avtat=0,i,j,bt1[20]={8,4,1},p[20],wt1[20],tat1[20],total=0,pos,temp;;
    float avg_wt,avg_tat;
    printf("TURNAROUND TIME ACCORDING TO FCFS......");
    wt[0]=0;  
    for(i=1;i<=3;i++)
    {
        wt[i]=0;
        for(j=0;j<i;j++)
            wt[i]+=bt[j];
    }
    printf("\nProcess\t\tBurst Time\tWaiting Time\tTurnaround Time");
    for(i=0;i<=3;i++)
    {
        tat[i]=bt[i]+wt[i];
        avwt+=wt[i];
        avtat+=tat[i];
        printf("\nP[%d]\t\t%d\t\t%d\t\t%d",i+1,bt[i],wt[i],tat[i]);
    }
    avwt/=i;
    avtat/=i;
    printf("\n\nAverage Waiting Time:%d",avwt);
    printf("\nAverage Turnaround Time:%d",avtat);
    printf("              \n");
    printf("TURNAROUND TIME ACCORDING TO SJF......");
    for(i=0;i<3;i++)
    {
        pos=i;
        for(j=i+1;j<3;j++)
        {
            if(bt1[j]<bt1[pos])
                pos=j;
        }
 
        temp=bt1[i];
        bt1[i]=bt1[pos];
        bt1[pos]=temp;
 
        temp=p[i];
        p[i]=p[pos];
        p[pos]=temp;
    }
 
    wt1[0]=0;          
    for(i=1;i<3;i++)
    {
        wt1[i]=0;
        for(j=0;j<i;j++)
            wt1[i]+=bt1[j];
 
        total+=wt1[i];
    }
 avg_wt=(float)total/i;     
    total=0;
 printf("\nProcess\t    Burst Time    \tWaiting Time\tTurnaround Time");
    for(i=0;i<3;i++)
    {
        tat1[i]=bt1[i]+wt1[i];     
        total+=tat1[i];
        printf("\np%d\t\t  %d\t\t    %d\t\t\t%d",i+1,bt1[i],wt1[i],tat1[i]);
    }
    avg_tat=(float)total/i;    
    printf("\n\nAverage Waiting Time=%f",avg_wt);
    printf("\nAverage Turnaround Time=%f\n",avg_tat);
 struct Process
{
   int pid1; 
   int bt3; 
};
bool comparison(Process a, Process b)
{
     return (a.bt3 < b.bt3);
}
void findWaitingTime(Process proc[], int n, int wt3[])
{
    wt3[0] = 0;
    for (int i = 1; i < n ; i++ )
        wt3[i] = proc[i-1].bt3 + wt3[i-1] ;
}
void findTurnAroundTime(Process proc[], int n,
                        int wt3[], int tat[])
{
    for (int i = 0; i < n ; i++)
        tat[i] = proc[i].bt3 + wt3[i];
}
void findavgTime(Process proc[], int n)
{
    int wt3[n], tat[n], total_wt = 0, total_tat = 0;
    findWaitingTime(proc, n, wt3);
    findTurnAroundTime(proc, n, wt3, tat);
    cout << "\nProcesses "<< " Burst time "
         << " Waiting time " << " Turn around time\n";
    for (int i = 0; i < n; i++)
    {
        total_wt = total_wt + wt3[i];
        total_tat = total_tat + tat[i];
        cout << " " << proc[i].pid1 << "\t\t"
             << proc[i].bt3 << "\t " << wt3[i]
             << "\t\t " << tat[i] <<endl;
    }
 
    cout << "Average waiting time = "
         << (float)total_wt / (float)n;
    cout << "\nAverage turn around time = "
         << (float)total_tat / (float)n;
}
int main()
{
    Process proc[] = {{1, 8}, {2, 4}, {3, 1}};
    int n = sizeof proc / sizeof proc[0];
    sort(proc, proc + n, comparison);
    cout << "Order in which process gets executed\n";
    for (int i = 0 ; i < n; i++)
        cout << proc[i].pid1 <<" ";
findavgTime(proc, n);
    return 0;
}
}
 
