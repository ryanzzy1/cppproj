

const int n = 2;

void p(int i )
{
    message msg;
    while(true){
        receive(box,msg);
	

	send(box,msg);
    }
}

void main()
{
    create mailbox(box);
    send(box,null);
    parbegin(p(1), p(2), ..., p(3));
}
