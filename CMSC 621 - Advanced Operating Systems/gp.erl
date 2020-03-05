%%-----------------------------------------------------------------
%%			Sandipan Dey
%%			CSEE - 621
%%-----------------------------------------------------------------
%% Gossiping with Erlang

-module(gp).

-export([runAlgo/1, runAlgo/2, runAlgo/3]).
-define(EPSILON, 0.1).
-define(STOPPROB, 0.5).
-define(MAXTIME(F, N), F * math:log(N)).
-define(WIDTH, 1200).  %% win size
-define(HEIGHT, 600).  %% win size
-define(LBOUND, 1).
-define(UBOUND(N), N). %math:log(N)). % N). 
-define(MARGIN, 36).
-define(CANVASCOL, white).
-define(DRAWLINE, false).
-define(PTSZ, 5).
-define(FPTSZ, 10).

%%   avgf(N data points) 
%%   Create N processes to simulate N nodes

runAlgo(N) ->
runAlgo(N, avg).

runAlgo(N, Algo) ->
runAlgo(N, Algo, 4).

runAlgo(N, Algo, F) ->
    I=gs:start(),
    Win=gs:create(window, I,
		  [{width, ?WIDTH}, {height, ?HEIGHT},
		   {title,"Gossip"}, {bg,white}, {map, true}]),
    gs:create(canvas, can1,Win,
	      [{x,?MARGIN},{y, ?MARGIN}, {width,?WIDTH - 2 * ?MARGIN},{height,?HEIGHT - 2 * ?MARGIN}, {bg,?CANVASCOL}]),
	%gs:create(canvas, can1,Win,
	%      [{x,0},{y, 0}, {width,erlang:max(?WIDTH, N)},{height,?HEIGHT}, {bg,?CANVASCOL}]),
	gs:create(polygon, can1, [{coords, [{0, 0}, {0, ?HEIGHT - 2 * ?MARGIN - 1}, {?WIDTH - 2 * ?MARGIN - 1, ?HEIGHT - 2 * ?MARGIN - 1}, {?WIDTH - 2 * ?MARGIN - 1, 0}]}, 
				{fill, ?CANVASCOL}]),
	gs:create(label, Win, [{label, {text, "0"}}, {width, 10}, {height,25},
			   {x, ?MARGIN}, {y,?HEIGHT - ?MARGIN + 1}, {bg,white}]),
	gs:create(label, Win, [{label, {text, io_lib:format("~p Nodes", [N])}}, {width, 60}, {height,25},
			   {x, erlang:min(?WIDTH - 2 * ?MARGIN, N + ?MARGIN)}, {y,?HEIGHT - ?MARGIN}, {bg,white}]),
	gs:create(label, Win, [{label, {text, io_lib:format("~p", [?UBOUND(N)])}}, {width, 100}, {height,25},
			   {x, ?MARGIN / 2}, {y, 5}, {bg,white}]),
	gs:create(label, Win, [{label, {text, "Values"}}, {width, ?MARGIN}, {height,25},
			   {x, 0}, {y, ?HEIGHT / 2}, {bg,white}]),
	%draw_lines(0),
    L = for(1, N, fun() -> spawn(fun() -> act(undefined) end) end),
    D = for(1, N, fun() -> genData(N, ?LBOUND, ?UBOUND(N)) end),
	draw_points(D),
	%lists:foreach(fun(PId) -> update_point(index_of(PId, L), lists:nth(index_of(PId, L), D), lists:nth(index_of(PId, L), D) / 2, N) end, L),
    %random:seed(44, 55, 66),
    MT = F * math:log(N),
    case Algo of
	avg -> A = avgf(D);
	sum -> A = sumf(D);
	min -> A = minf(D);
	max -> A = maxf(D);
	true -> A = avgf(D)
    end,
    put("ACTUALAGG",A),
	put("MSE",0),
    io:format("sum = [~p], count = [~p], avg = [~p], min = [~p], max = [~p], max-iteration = [~p] ~n", 
				[sumf(D), length(D), avgf(D), minf(D), maxf(D), MT]),
    for1(2, N, fun(I) -> lists:nth(I, L) ! {init, pristine, L, lists:nth(I, D), MT, A, Algo, self()} end),
    Pid1 = lists:nth(1, L),
    Pid1 ! {init, contagious, L, lists:nth(1, D), MT, A, Algo, self()},
    gs:config(Win, {map, true}),
    wait(Win, L).
	%,    
    %readFinalData(N),
    %io:format("data in nodes: ~n"),	
    %lists:foreach(fun(Pid) -> io:format("[~p] ", [get(Pid)]) end, L),
    %io:format("~n done! ~n").	
    
    %Max = erlang:system_info(process_limit),
    %io:format("Maximum allowed processes:~p~n",[Max])

act(undefined) ->

    receive

	{init, S, L, D, MT, A, Algo, PPID}->
			   N = length(L),
			   random:seed(now()),
			   %put("PERM", randomize(lists:seq(1, N))),
			   %put("LASTCHOSEN", 1), %I rem N + 1),
			   put("PIDS", L),
			   put("STATE", S),
			   put("DATA", D),
			   put("Time", 0),
     		   put("MAXTIME", MT),
			   put("ACTUALAGG", A),
			   put("ALGO", Algo),
			   put("PPID", PPID),
			   io:format("[~p]: node [~p] state [~p] data [~p] #neighbors [~p] ~n",
							[get("Time"), self(), get("STATE"), get("DATA"), length(L)]),
			   act(S)

    end;

act(pristine) ->

    Time = get("Time"),	
    MaxTime = get("MAXTIME"),
    Algo = get("ALGO"),	
    L = get("PIDS"),
	PPid = get("PPID"),
    
	receive

	{compute, PId, D}->
				   D1 = get("DATA"),
				   D2 = algo(Algo, D, D1),
				   put("DATA", D2),
				   put("STATE", contagious),
				   PPid!{self(), D1, D2, contagious},
	    		   io:format("[~p]: node [~p] state [~p] data [~p] : gossiping+updating node [~p] to [~p] ~n", 
							[get("Time"), self(), get("STATE"), D1, PId, get("DATA")]),
				   PId!{update, D2},
   				   if 
					(Time =< MaxTime) ->  act(get("STATE"));
					true ->	void
				   end;

	{Other}	->		if 
					(Time =< MaxTime) ->  act(get("STATE"));
					true ->	void
				   end

    end;			    

act(contagious) ->
	
    L = get("PIDS"),
	N = length(L),
	D1 = get("DATA"),
	Algo = get("ALGO"),
	PPid = get("PPID"),
	Time = get("Time"),
	put("Time", Time + 1),
	MaxTime = get("MAXTIME"),
	A = get("ACTUALAGG"),
    Neighbor = random:uniform(N),
	%Perm = get("PERM"),
	%Cur = get("LASTCHOSEN") rem N + 1,
	%Neighbor = lists:nth(Cur, Perm), 
	if
		Time =< MaxTime ->	
			Pid = lists:nth(Neighbor, L),
			if
				(self() =:= Pid) ->
					%Cur1 = Cur rem N + 1, 
					Nb = Neighbor rem N + 1; %lists:nth(Cur1, Perm);
				true ->
					%Cur1 = Cur, 
					Nb = Neighbor
			end,
			Pid1 = lists:nth(Nb, L),
			%put("LASTCHOSEN", Cur1),
			io:format("[~p]: node [~p] state [~p] data [~p] : choosing neighbor as node [~p] and sending data ~n", 
											[get("Time"), self(), get("STATE"), D1, Pid1]),
			Pid1!{compute, self(), D1};
		true -> 
			PPid!{self(), get("DATA")}
	end,
	receive		
		{update, D} ->     
				   Prob = random:uniform(N) / N,	%0.5 * random:uniform(N) / N,
				   %io:format("prob ~p ~p ~p ~p ~n", [abs(D1 - D), ?EPSILON, Prob, ?STOPPROB]),
				   if				   
					((abs(D1 - A) < ?EPSILON) andalso (Prob =< ?STOPPROB)) ->  put("STATE", infected);
					true ->  put("STATE", contagious)
				   end,
	 	   		   put("DATA", D),
				   PPid!{self(), D1, D, get("STATE")},
				   io:format("[~p]: node [~p] state [~p] data [~p] : done gossiping, updating to [~p] ~n", 
					[get("Time"), self(), get("STATE"), D1, get("DATA")]),
				   if 
					(Time =< MaxTime) ->  act(get("STATE"));
					true ->	void %PPid!{self(), get("DATA")}
				   end;
  
 	        {compute, PId, D}->
				   D2 = algo(Algo, D, D1),
				   put("DATA", D2),
				   PPid!{self(), D1, D2, contagious},
				   % put("STATE", contagious),
    			   io:format("[~p]: node [~p] state [~p] data [~p] : gossiping+updating node [~p] to [~p] ~n", 
						[get("Time"), self(), get("STATE"), D1, PId, get("DATA")]),
				   PId!{update, D2},
				   if 
					(Time =< MaxTime) ->  act(get("STATE"));
					true ->	void %PPid!{self(), get("DATA")}
				   end

		%{Other} ->	   if 
		%			(Time =< MaxTime) ->  act(get("STATE"));
		%			true ->	PPid!{self(), get("DATA")}
		%		   end

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
    L = get("PIDS"),
	PPid = get("PPID"),
	%io:format("Node ~p with state ~p ~n", [self(), get("STATE")]),

       	receive
	    {compute, PId, D}->
				   D1 = get("DATA"),
				   D2 = algo(Algo, D, D1),
				   put("DATA", D2),
				   put("STATE", contagious),
    			   io:format("[~p]: node [~p] state [~p] data [~p] : gossiping+updating node [~p] to [~p] ~n", 
						[get("Time"), self(), get("STATE"), D1, PId, get("DATA")]),				   
				   PPid!{self(), D1, D2, get("STATE")},
				   PId!{update, D2},
   				   if 
				   (Time =< MaxTime) ->  act(get("STATE"));
				    true ->	void %PPid!{self(), get("DATA")}
				   end

	    %{Other} ->	   if 
	    %				(Time =< MaxTime) ->  act(get("STATE"));
	    %				true ->	PPid!{self(), get("DATA")}
	    %			   end

       	end.


for(N, N, F) -> [F()];
for(I, N, F) -> [F()|for(I+1, N, F)].

for1(N, N, F) -> [F(N)];
for1(I, N, F) -> [F(I)|for1(I+1, N, F)].

avgf(L) -> sumf(L) / length(L).
sumf([H|T]) -> H + sumf(T);
sumf([]) -> 0.
minf([H|[H1|T]]) -> if H < H1 -> minf([H|T]); true -> minf([H1|T]) end;
minf([H]) -> H.
maxf([H|[H1|T]]) -> if H > H1 -> maxf([H|T]); true -> maxf([H1|T]) end;
maxf([H]) -> H.

uniform(A, B, N) -> A + (B - A) * random:uniform(N) / N.

genData(N, A, B) ->  uniform(A, B, N).

algo(Type, D1, D2) ->
case Type of
	sum -> (D1 + D2);
	avg -> (D1 + D2) / 2.0;
	min -> if (D1 < D2) -> D1; true -> D2 end;
	max -> if (D1 > D2) -> D1; true -> D2 end;
	true -> (D1 + D2)
end.

readFinalData(0) ->
	io:format("done reading ~n");
readFinalData(N) ->
receive
	{Pid, Data} -> put(Pid, Data), 
		       readFinalData(N - 1)
	% {Other} ->  void
end.

update_point(X, Y1, Y2, N, S) ->
	draw_point(X, Y1, ?CANVASCOL, N, ?PTSZ),
	draw_point(X, Y2, color(Y2, ?LBOUND, ?UBOUND(N), S), N, ?PTSZ).

final_update_point(X, Y, N) ->
	draw_point(X, Y, red, N, ?FPTSZ).	
	
draw_points(D) ->
	N = length(D),
	for1(1, N, fun(X) -> Y = lists:nth(X, D), C = color(Y, ?LBOUND, ?UBOUND(N), pristine), draw_point(X, Y, C, N, ?PTSZ) end).

draw_point(X, Y, Color, N, Ptsz) ->
    XStart = 0,
	YStart = ?HEIGHT - 2 * ?MARGIN,
	F = (?HEIGHT - 2 * ?MARGIN) / (?UBOUND(N) - ?LBOUND),
	if
	(?DRAWLINE =:= true) ->
		gs:create(line, can1, [{coords, [{XStart + X, YStart}, {XStart + X, YStart - Y * F}]}, {fg, Color}]);
	true ->
		gs:create(oval, can1, [{coords, [{XStart + X, YStart - Y * F}, {XStart + X + Ptsz, YStart - Y * F + Ptsz}]}, {fill, Color}])
	end.
	
color(Y, A, B, S) ->	
	if 
		(S =:= pristine) ->
		G1 = 255, R1 = 255 - trunc(255 * (Y - A)/(B - A)), B1 = trunc(255 * (Y - A)/(B - A));
		(S =:= contagious) ->
		R1 = 255, B1 = 255 - trunc(255 * (Y - A)/(B - A)), G1 = trunc(255 * (Y - A)/(B - A));
		true ->
		B1 = 255, G1 = 255 - trunc(255 * (Y - A)/(B - A)), R1 = trunc(255 * (Y - A)/(B - A))
	end, 	
	R = R1, BL = B1, G = G1,
	if 
	(G1 < 0) -> G = 0;
	(B1 < 0) -> BL = 0;
	(R1 < 0) -> R = 0;
	true -> void
	end, 	
	{R, G, BL}.
	
index_of(Item, List) -> index_of(Item, List, 1).
index_of(_, [], _)  -> not_found;
index_of(Item, [Item|_], Index) -> Index;
index_of(Item, [_|Tl], Index) -> index_of(Item, Tl, Index+1).

perms([]) -> [[]];
perms(L) -> [[H|T] || H <- L, T <- perms(L--[H])].

wait(Win, L) ->
    
	receive
	
	{PId, D1, D2, S} ->
		update_point(index_of(PId, L), D1, D2, length(L), S),
	    wait(Win, L);
	
	{PId, D} ->
		final_update_point(index_of(PId, L), D, length(L)),
		A = get("ACTUALAGG"),
		P = (D - A),
		Q = P * P,
		MS = get("MSE"),
		MS1 = MS + Q,
		io:format("MSE = [~p]",[MS]),
		put("MSE",MS1),
	    wait(Win, L);

	{gs,_Id,destroy,_Data,_Arg} -> 
	    exit(normal);

	{gs,_, _,[],["Exit",_]} ->
	    exit(normal);
	
	_ ->
	    wait(Win, L)
		
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
