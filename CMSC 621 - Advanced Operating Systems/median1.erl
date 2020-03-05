%%-----------------------------------------------------------------
%%			Sandipan Dey
%%			CSEE - 621
%%-----------------------------------------------------------------
%% Median with Gossip

-module(median1).

-export([runAlgo/1, runAlgo/2, runAlgo/3]).

runAlgo(N) ->
runAlgo(N, med).

runAlgo(N, Algo) ->
runAlgo(N, Algo, 20).

runAlgo(N, Algo, F) ->

    L = for(1, N, fun() -> spawn(fun() -> act(undefined) end) end),
    random:seed(now()),
	D = randomize(lists:seq(1, N)),
	LT = [],
	GT = [],
    T = 0.1,
    TS = 0.5,
    MT = F * math:log(N),
    case Algo of
	med -> A = medf(D);
	sum -> A = sumf(D);
	min -> A = minf(D);
	max -> A = maxf(D);
	true -> A = medf(D)
    end,
    
    io:format("sum = [~p], count = [~p], median = [~p], min = [~p], max = [~p], max-iteration = [~p] ~n", 
				[sumf(D), length(D), medf(D), minf(D), maxf(D), MT]),
    Pid1 = lists:nth(1, L),
    Pid1 ! {init, contagious, L, lists:nth(1, D), N, T, TS, MT, A, Algo, self(),LT,GT},
    for1(2, N, fun(I) -> lists:nth(I, L) ! {init, pristine, L, lists:nth(I, D), N, T, TS, MT, A, Algo, self(),LT,GT} end),
	if
		(N rem 2 =:=0)->
			put("EVENMEDIAN",0),
			put("COUNTER",0),
			wait();
	true->void
	end.
    %,    
    %readFinalData(N),
    %io:format("data in nodes: ~n"),	
    %lists:foreach(fun(Pid) -> io:format("[~p] ", [get(Pid)]) end, L),
    %io:format("~n done! ~n").	
    
    %Max = erlang:system_info(process_limit),
    %io:format("Maximum allowed processes:~p~n",[Max])
wait()->
receive
	{recordmedian,Val}->
		Cnt =get("COUNTER"),
		Val1 = get("EVENMEDIAN"),
		put("COUNTER",Cnt+1),
		put("EVENMEDIAN",Val+Val1),
		Cnt1 = get("COUNTER"),
		if 
			(Cnt1 < 2)->
				io:format("~n CALLING WAIT() ~n"),
				wait();
		true->
			A1 = get("EVENMEDIAN"),
			A2 = A1/2,
			put("EVENMEDIAN",A2),
			io:format("~n ~n ~n ~n ~n ~n ~n ~n MEDIAN MEDIAN MEDIAN MEDIAN = ~p ~n ~n ~n ~n ~n ~n",[A2]),
			Li = get("PIDS"),
			N1 = length(Li),
			for(1,N1,fun(I)->exit(lists:nth(I),kill) end),
			exit(normal)
		
			end;
	_ ->
	    wait()
		  end.


act(undefined) ->

    receive

	{init, S, L, D, N, T, TS, MT, A, Algo, PPID,LT,GT}-> 
			   put("PIDS", L),
			   put("STATE", S),
    random:seed(now()),
	put("NUMDATA", N),
			   put("DATA", D),
		           put("EPSILON", T),
    			   put("STOPPROB", TS),
			   put("Time", 0),
     			   put("MAXTIME", MT),
			   put("ACTUALAGG", A),
			   put("ALGO", Algo),
			   put("PPID", PPID),
			   put("LTHAN",LT),
			   put("GTHAN",GT),
			   io:format("[~p]: node [~p] state [~p] data [~p] #neighbors [~p] LT [~p] GT [~p]~n",
							[get("Time"), self(), get("STATE"), get("DATA"), get("NUMDATA"),get("LTHAN"),get("GTHAN")]),
			   act(S)

    end;

act(pristine) ->

    Time = get("Time"),	
    MaxTime = get("MAXTIME"),
    Algo = get("ALGO"),	
    receive

	{compute, PId, D}->
				   D1 = get("DATA"),
				   algo(Algo, D, D1,PId,self()),
				   
				   put("STATE", contagious),
	    			   io:format("[~p]: node [~p] state [~p] data [~p] LT [~p] GT[~p]: gossiping with node [~p] ~n", 
							[get("Time"), self(), get("STATE"), D1, get("LTHAN"),get("GTHAN"), PId]),
				   PId!{update, D1, self()},
   				   if 
					(Time =< MaxTime) ->  act(get("STATE"));
					true ->	void
				   end;

	{Other}	->		   if 
					(Time =< MaxTime) ->  act(get("STATE"));
					true ->	void
				   end

    end;	
	

act(contagious) ->
	
    L = get("PIDS"),
	N = get("NUMDATA"),
	D1 = get("DATA"),
	Algo = get("ALGO"),
	PPid = get("PPID"),
	Time = get("Time"),
	put("Time", Time + 1),
	MaxTime = get("MAXTIME"),
	A = get("ACTUALAGG"),
     	Neighbor = random:uniform(2 * N) rem N + 1,
	if
		Time =< MaxTime ->	
			Pid = lists:nth(Neighbor, L),
			if
				(self() =:= Pid) ->
					Nb = Neighbor rem N + 1;
				true ->
					Nb = Neighbor
			end,
			Pid1 = lists:nth(Nb, L),
			io:format("[~p]: node [~p] state [~p] data [~p] LT [~p] GT[~p]: choosing neighbor as node [~p] and sending data ~n", 
											[get("Time"), self(), get("STATE"), D1,get("LTHAN"),get("GTHAN"), Pid1]),
			Pid1!{compute, self(), D1};
		true -> 
			void
	end,
	receive		
		{update, D, PId} ->     
				   T = get("EPSILON"),
				   TS = get("STOPPROB"),
				   Prob = random:uniform(N) / N,	%0.5 * random:uniform(N) / N,
				   %io:format("prob ~p ~p ~p ~p ~n", [abs(D1 - D), T, Prob, TS]),
				   if				   
					((((D1 - A)*(D1 - A)) < T) andalso (Prob =< TS)) ->  put("STATE", contagious);
					true ->  put("STATE", contagious)
				   end,
	 	   		   %put("DATA", D),
				   algo(Algo,D,D1,PId,self()),
				   io:format("[~p]: node [~p] state [~p] data [~p] LT [~p] GT [~p]: done gossiping ~n", 
					[get("Time"), self(), get("STATE"),D1,get("LTHAN"),get("GTHAN")]),
				   if 
					(Time =< MaxTime) ->  act(get("STATE"));
					true ->	PPid!{self(), get("DATA")}
				   end;
  
 	        {compute, PId, D}->
				   algo(Algo, D, D1,PId,self()),
				   %put("DATA", D2),
				   % put("STATE", contagious),
	    			   io:format("[~p]: node [~p] state [~p] data [~p] LT [~p] GT [~p]: gossiping with node [~p] ~n", 
						[get("Time"), self(), get("STATE"), D1,get("LTHAN"),get("GTHAN"), PId]),
				   PId!{update, D1, self()},
				   if 
					(Time =< MaxTime) ->  act(get("STATE"));
					true ->	PPid!{self(), get("DATA")}

				   end;

		%{Other} ->	   if 
		%			(Time =< MaxTime) ->  act(get("STATE"));
		%			true ->	PPid!{self(), get("DATA")}
		%		   end


		%after 100 ->      
		%		   io:format("Node ~p with state ~p and data ~p, no message to process ~n", 
		%				[self(), get("STATE"), get("DATA")]),
		%		   act(get("STATE")) 

	%end;

	{iamthemedian,Val} ->
		put("DATA",Val),
		io:format("Node [~p] got the median =~p and hence exiting",[PPid,Val])
		%exit(PPid,kill)
	end,
		
	if
		(Time > MaxTime) ->
			{_, Time1} = statistics(runtime),
		    	{_, Time2} = statistics(wall_clock),
		    	U1 = Time1 * 1000 / N,
		    	U2 = Time2 * 1000 / N,
			io:format("total time taken: [~p] ([~p]) usec. actual [~p] : [~p] ~n", [U1, U2, get("ALGO"), get("ACTUALAGG")]);
		true ->
			void
	end;

act(Other) ->

        Time = get("Time"),	
    	MaxTime = get("MAXTIME"),
	Algo = get("ALGO"),
	PPid = get("PPID"),
	%io:format("Node ~p with state ~p ~n", [self(), get("STATE")]),

       	receive
	    {compute, PId, D}->
				   D1 = get("DATA"),
				   algo(Algo, D, D1, PId,self()),
				   %put("DATA", D2),
				   put("STATE", contagious),
	    			   io:format("[~p]: node [~p] state [~p] data [~p] LT [~p] GT[~p]: gossiping with node [~p] ~n", 
						[get("Time"), self(), get("STATE"), D1, get("LTHAN"),get("GTHAN"),PId]),
				   PId!{update, D1, self()},
   				   if 
					(Time =< MaxTime) ->  act(get("STATE"));
					true ->	PPid!{self(), get("DATA")}
				   end

	    %{Other} ->		   if 
	    %				(Time =< MaxTime) ->  act(get("STATE"));
	    %				true ->	PPid!{self(), get("DATA")}
	    %			   end

       	end.


for(N, N, F) -> [F()];
for(I, N, F) -> [F()|for(I+1, N, F)].

for1(N, N, F) -> [F(N)];
for1(I, N, F) -> [F(I)|for1(I+1, N, F)].

%avgf(L) -> sumf(L) / length(L).
medf(L) -> 
	N2 = length(L),
	T1 = lists:nth(getLength(L),lists:sort(L)),
	T2 = lists:nth(getLength(L)+1,lists:sort(L)),
	T3 = (T1 + T2)/2,
	if(N2 rem 2 > 0)->
	lists:nth(getLength(L),lists:sort(L));
	true-> T3
	end.
	
		
getLength(L) -> ceiling(length(L)/2).

ceiling(X) ->
    T = erlang:trunc(X),
    case (X - T) of
        Neg when Neg < 0 -> T;
        Pos when Pos > 0 -> T + 1;
        _ -> T
    end.




sumf([H|T]) -> H + sumf(T);
sumf([]) -> 0.
minf([H|[H1|T]]) -> if H < H1 -> minf([H|T]); true -> minf([H1|T]) end;
minf([H]) -> H.
maxf([H|[H1|T]]) -> if H > H1 -> maxf([H|T]); true -> maxf([H1|T]) end;
maxf([H]) -> H.

uniform(A, B, N) -> A + (B - A) * random:uniform(N) / N.

genData(N) ->  uniform(1, math:log(N), N).
%D2=self data

algo(Type, D1, D2,PId,SelfId) ->
LessThan = get("LTHAN"),
GreaterThan = get("GTHAN"),
X = lists:member(PId,LessThan),
Y = lists:member(PId,GreaterThan),
if
	(D1 < D2)->
		if
			(X =:= false)->
		put("LTHAN",lists:append(LessThan,[PId]));
			true -> void
		end;
	true ->
		if
			(Y =:= false)->
		put("GTHAN",lists:append(GreaterThan,[PId]));
			true -> void
		end
end,
Less = get("LTHAN"),
Grt = get("GTHAN"),
Num = get("NUMDATA"),
A = floor(Num/2),
B = length(Less),
C = length(Grt),
E = A - 1,
io:format("Node = ~p A= ~p B=~p C=~p ~n",[SelfId,A,B,C]),
if(Num rem 2 > 0)->
if(A =:= B andalso A=:=C )->
io:format("~n ~n ~n ~n ~n ~n ~n Node= ~p is the median ~n ~n ~n ~n ~n ~n ~n",[SelfId]),
cprof:pause();
true->void
end;
true->
if((B=:=A andalso C=:=E) or (B=:=E andalso C=:=A))->
Pid = get("PPID"),
io:format("NODE ~p CONTACTING THE PARENT~n",[SelfId]),
Pid ! {recordmedian,D2},
exit(normal);
true->void
end
end.


floor(X) ->
    T = erlang:trunc(X),
    case (X - T) of
        Neg when Neg < 0 -> T - 1;
        Pos when Pos > 0 -> T;
        _ -> T
    end.

	random_partition(List) ->
	random_partition(List, [], []).

	random_partition([], Left, Right) ->
		{Left,Right};
	random_partition([X|Xs], Left, Right) ->
		U = random:uniform(),
		if U >= 0.5 ->
		random_partition(Xs, Left, [X|Right]);
		U < 0.5 ->
		random_partition(Xs, [X|Left], Right)
		end.

	randomize([]) ->
		[];
	randomize([X]) ->
		[X];
	randomize(Bigger) ->
		{Left, Right} = random_partition(Bigger),
		randomize(Left) ++ randomize(Right).
