%%-----------------------------------------------------------------
%%			Sandipan Dey
%%			CSEE - 621
%%-----------------------------------------------------------------
%% Distributed Outlier Detection with erlang

-module(outlier).
-define(NODEFILE, "nodes").
-define(DATAFILE, "/data").
-define(DATAFILE1, "/data1").
-define(STATEFILE, "/state").
-define(LOCALOUTLIERFILE, "/outliers").
-define(LOCALOUTLIER1FILE, "/outliers1").
-define(ALGOFILE, "algo").
-define(EPSILON, 0.1).
-define(STOPPROB, 0.5).
-define(NN, 10).
-define(MAXTIME(F, No), F * math:log(No)).
-define(PIDFILE(Pid, File), string:concat(erlang:element(2, regexp:gsub(erlang:element(2, regexp:gsub(pid_to_list(Pid),"<","")), ">", "")), File)).
-define(READSTATEFILE(Pid), file2term(?PIDFILE(Pid, ?STATEFILE))).
-define(READLOCALOUTLIERFILE(Pid), file2term(?PIDFILE(Pid, ?LOCALOUTLIERFILE))).
-define(READSTATE(Pid), lists:nth(1, ?READSTATEFILE(Pid))).
-define(READLOCALOUTLIER(Pid), lists:nth(1, ?READLOCALOUTLIERFILE(Pid))).
-define(WRITELOCALOUTLIERFILE(Pid, D), term2file(?PIDFILE(Pid, ?LOCALOUTLIERFILE), D)).
-define(WRITELOCALOUTLIER1FILE(Pid, D), unconsult(?PIDFILE(Pid, ?LOCALOUTLIER1FILE), D)).
-define(WRITESTATEFILE(Pid, S), term2file(?PIDFILE(Pid, ?STATEFILE), S)).
-export([runAlgo/1, runAlgo/2, runAlgo/3, runAlgo/4, runAlgo/5, find_outliers/2, to_tuple/1]).

%%   Create No processes to simulate No nodes

runAlgo(No) ->
runAlgo(No, No).

runAlgo(No, M) ->
runAlgo(No, M, M).

runAlgo(No, M, N) ->
runAlgo(No, M, N, 5).

runAlgo(No, M, N, K) ->
runAlgo(No, M, N, K, 4).

runAlgo(No, M, N, K, F) ->

    L = for(1, No, fun() -> spawn(fun() -> act(undefined) end) end),
    term2file(?NODEFILE, L),
    mkdir(L),	
    random:seed(now()),
	genData(M, N, K, L),
    genState(L),	
    lists:foreach(fun(Pid) -> computeLocalTopKOutliers(Pid, K) end, L),
    computeGlobalTopKOutliers(L, K, erlang:trunc(?MAXTIME(F, N))),
    O1 = lists:nth(2, file2term(?ALGOFILE)),
    lists:foreach(fun(Pid) -> Pid!{init} end, L),
    receive
    after 5000 -> void 	
    end,
    io:format("values at different nodes: ~n"),
    lists:foreach(fun(Pid) -> io:format(" ~p", ?READLOCALOUTLIERFILE(Pid)) end, L),
    io:format("~n"),
    computeGlobalTopKOutliers(L, K, erlang:trunc(?MAXTIME(F, No))),
    O2 = lists:nth(2, file2term(?ALGOFILE)),
    io:format("~n actual top [~p] outlier set: [~p]", [K, O1]).

act(undefined) ->

    receive

	{init} -> 	   put("Time", 0), 	
			   S = ?READSTATE(self()),
			   io:format("[~p]: node [~p] state [~p] ~n", [get("Time"), self(), S]),
			   act(S)

    end;

act(pristine) ->

    Time = get("Time"),	
    A = file2term(?ALGOFILE),
    K = lists:nth(1, A),
    MaxTime = lists:nth(3, A),
	
    receive

	{compute, PId, O}->
				   O1 = ?READLOCALOUTLIER(self()),
				   O2 = combine_outliers(O, O1, K),
				   ?WRITELOCALOUTLIER1FILE(self(), [O2]),
				   ?WRITELOCALOUTLIERFILE(self(), [O2]),
				   ?WRITESTATEFILE(self(), [contagious]),
				   State = ?READSTATE(self()),
	    			   io:format("[~p]: node [~p] state [~p] local outliers ~n [~p] : ~n gossiping+updating node [~p] to [~p] ~n", 
							[get("Time"), self(), State, O1, PId, O2]),
				   PId!{update, O2},
   				   if 
					(Time =< MaxTime) ->  act(State);
					true ->	void
				   end;

	{Other}	->		   if 
					(Time =< MaxTime) ->  act(?READSTATE(self()));
					true ->	void
				   end
    end;			    

act(contagious) ->
	
        L = file2term(?NODEFILE),
	No = length(L),
	State = ?READSTATE(self()),
	O1 = ?READLOCALOUTLIER(self()),
        Time = get("Time"),	
        A = file2term(?ALGOFILE),
        K = lists:nth(1, A),	
        MaxTime = lists:nth(3, A),	
	put("Time", Time + 1),
     	Neighbor = random:uniform(No),
	if
		Time =< MaxTime ->	
			Pid = lists:nth(Neighbor, L),
			if
				(self() =:= Pid) ->
					Nb = Neighbor rem No + 1;
				true ->
					Nb = Neighbor
			end,
			Pid1 = lists:nth(Nb, L),
			io:format("[~p]: node [~p] state [~p] local [~p] outliers ~n [~p] : ~n choosing neighbor as node [~p] and sending knowledge ~n", 
											[get("Time"), self(), State, K, O1, Pid1]),
			Pid1!{compute, self(), O1};
		true -> 
			void
	end,
	receive		
		{update, O} ->     
				   Prob = random:uniform(No) / No,	%0.5 * random:uniform(N) / N,
				   %io:format("prob ~p ~p ~p ~p ~n", [abs(D1 - D), T, Prob, TS]),
				   AlreadyKnows = isequal(O1, O),
				   if				   
					(AlreadyKnows =:= true andalso (Prob =< ?STOPPROB)) ->  
						?WRITESTATEFILE(self(), [infected]);
					true ->  
						?WRITESTATEFILE(self(), [contagious])
				   end,
				   ?WRITELOCALOUTLIER1FILE(self(), [O]),
				   ?WRITELOCALOUTLIERFILE(self(), [O]),
				   io:format("[~p]: node [~p] state [~p] local [~p] outliers ~n [~p] : ~n done gossiping, updating to [~p] ~n", 
					[get("Time"), self(), State, K, O1, ?READLOCALOUTLIER(self())]),
				   if 
					(Time =< MaxTime) ->  act(?READSTATE(self()));
					true ->	void
				   end;
  
 	        {compute, PId, O}->
				   O2 = combine_outliers(O, O1, K),
				   ?WRITELOCALOUTLIER1FILE(self(), [O2]),
				   ?WRITELOCALOUTLIERFILE(self(), [O2]),
				   State = ?READSTATE(self()),
				   Data = ?READLOCALOUTLIER(self()),
	    			   io:format("[~p]: node [~p] state [~p] local [~p] outliers ~n [~p] : ~n gossiping+updating node [~p] to [~p] ~n", 
						[get("Time"), self(), State, K, O1, PId, Data]),
				   PId!{update, O2},
				   if 
					(Time =< MaxTime) ->  act(State);
					true ->	void

				   end

	end,
	if
		(Time > MaxTime) ->
			{_, Time1} = statistics(runtime),
		    	{_, Time2} = statistics(wall_clock),
		    	U1 = Time1 * 1000 / No,
		    	U2 = Time2 * 1000 / No,
			A = file2term(?ALGOFILE),
			ActualOutlierSet = lists:nth(2, A),
			io:format("total time taken: [~p] ([~p]) usec. actual outlier set : [~p] ~n", [U1, U2, ActualOutlierSet]);
		true ->
			void
	end;

act(Other) ->

        Time = get("Time"),	
        A = file2term(?ALGOFILE),
        K = lists:nth(1, A),	
        MaxTime = lists:nth(3, A),	

       	receive
	    {compute, PId, O}->
			 	   O1 = ?READLOCALOUTLIER(self()),
				   O2 = combine_outliers(O, O1, K),
				   ?WRITELOCALOUTLIER1FILE(self(), [O2]),
				   ?WRITELOCALOUTLIERFILE(self(), [O2]),
				   ?WRITESTATEFILE(self(), [contagious]),
				   State = ?READSTATE(self()),
				   Data = ?READLOCALOUTLIER(self()),
	    			   io:format("[~p]: node [~p] state [~p] local [~p] outliers ~n [~p] : ~n gossiping+updating node [~p] to [~p] ~n", 
						[get("Time"), self(), State, K, O1, PId, Data]),
				   PId!{update, O2},
   				   if 
					(Time =< MaxTime) ->  act(State);
					true ->	void
				   end

       	end.

for(N, N, F) -> [F()];
for(I, N, F) -> [F()|for(I+1, N, F)].

for1(N, N, F) -> [F(N)];
for1(I, N, F) -> [F(I)|for1(I+1, N, F)].

createtuple(I, I, F) -> {F(I)}; 
createtuple(I, J, F) -> erlang:append_element(createtuple(I, J - 1, F), F(J)). 

addtuples(T1, T2) ->
createtuple(1, size(T1), fun(I) -> erlang:element(I, T1) +  erlang:element(I, T2) end).

isequal(L1, L2) ->
lists:sort(L1) =:= lists:sort(L2).

find_outliers(L, K) ->
	M = length(L),
	N = size(lists:nth(1, L)),
	LO = for1(1, N, fun(J) ->  for1(1, K, fun(I) -> erlang:element(J, lists:nth(I, L)) end) end),
	LD = for1(1, N, fun(J) ->  for1(K + 1, M, fun(I) -> erlang:element(J, lists:nth(I, L)) end) end),
	%DistO =	for1(1, N, fun(I) -> 
	%			L1 = lists:nth(I, LO),
	%			compute_freq([{X, 1} || X <- L1])
	%		end),
	DistD =	for1(1, N, fun(I) -> 
				L2 = lists:nth(I, LD),
				compute_freq([{Y, 1} || Y <- L2])
			end),
	%[HO, HD] = [h_entropy(DistO, K), h_entropy(DistD, M - K)],
	HD = h_entropy(DistD, M - K),
	HDKO	=	{HD, LO},
	io:format(" LO: ~p ~n HD: ~p ~n", [LO, HD]),
	ListHDKO = 	for1(1, M - K, fun(I) -> 
					%DistO1 = 	for1(1, N, fun(J) ->	
					%				[L1, L2] = [lists:nth(J, LO), lists:nth(J, LD)],
					%				[L11, L12] = swap_elements(1, L1, I, L2),
					%				compute_freq([{X, 1} || X <- L11]) 
					%			end),
					DistD1_LO1 = 	for1(1, N, fun(J) ->	
									[L1, L2] = [lists:nth(J, LO), lists:nth(J, LD)],
									[L11, L12] = swap_elements(1, L1, I, L2),
									{compute_freq([{X, 1} || X <- L12]), L11} 
								end),
					DistD1	=	[erlang:element(1, X) || X <- DistD1_LO1],
					LO1	=	[erlang:element(2, X) || X <- DistD1_LO1],
					%[HO1, HD1] = [h_entropy(DistO1, K), h_entropy(DistD1, M - K)],
					%io:format(" LO: ~p ~n HD: ~p ~n", [LO1, HD1]),
					HD1 = h_entropy(DistD1, M - K),
					{HD1, LO1} 
				end),			
	{H, O} = min_entropy([HDKO|ListHDKO]),
	{H, to_tuple(O)}.
	% io:format(" ~p ~p ~n", [H, to_tuple(O)]).

to_tuple(L)	->					
	N = length(L),
	K = length(lists:nth(1, L)),
	for1(1, K, fun(I)	->	
		createtuple(1, N, fun(J) -> lists:nth(I, lists:nth(J, L)) end)
	end).
	
min_entropy([H|[H1|T]]) -> {E, O} = H,	{E1, O1} = H1,
	Comp_res = compare_entropy(E, E1),
	if Comp_res < 0 -> min_entropy([H|T]); true -> min_entropy([H1|T]) end;
min_entropy([H]) -> H.

compare_entropy(H1, H2) ->	
	S1 = lists:sum(H1),
	S2 = lists:sum(H2),
	if
		S1 < S2	->	R = -1;
		true	->	R = 1
	end,
	R.

swap_elements(Pos1, List1, Pos2, List2) ->
El1 = lists:nth(Pos1, List1),
El2 = lists:nth(Pos2, List2),
L1 = replace_element(Pos1, El2, List1),
L2 = replace_element(Pos2, El1, List2),
[L1, L2].
	
replace_element(1, El, List) -> 
    [_ | TheRest] = List, [El|TheRest]; 
replace_element(ElemPos, El, List) when length(List) == ElemPos -> 
    [_ | TheRest] = lists:reverse(List), lists:reverse([El|TheRest]); 
replace_element(ElemPos, El, List) -> 
    {ListA, ListB} = lists:split(ElemPos - 1, List), 
    [_, ElemB | ListC] = ListB, 
    ListResB = [ElemB | ListC], 
    ListA ++ [El|ListResB]. 
	
remove_element(1, List) -> 
    [_ | TheRest] = List, TheRest; 
remove_element(ElemPos, List) when length(List) == ElemPos -> 
    [_ | TheRest] = lists:reverse(List), lists:reverse(TheRest); 
remove_element(ElemPos, List) -> 
    {ListA, ListB} = lists:split(ElemPos - 1, List), 
    [_, ElemB | ListC] = ListB, 
    ListResB = [ElemB | ListC], 
    ListA ++ ListResB. 

h_entropy(D, N) ->
	for1(1, length(D), fun(I) -> L = lists:nth(I, D),
	lists:sum([-(erlang:element(2, X) / N) * math:log(erlang:element(2, X) / N) || X <- L]) end). 
								 
compute_freq(KV) -> compute_freq(KV, dict:new()).

compute_freq([{Key,Val}|T], D0) ->
    case dict:find(Key, D0) of
	{ok, L} -> compute_freq(T, dict:store(Key, L + 1, D0));
	error   -> compute_freq(T, dict:store(Key, Val, D0))
    end;
compute_freq([], D) ->
    dict:to_list(D).

uniform(A, B, N) -> erlang:trunc(A + (B - A) * random:uniform(N) / N).

mkdir(L) ->
lists:foreach(fun(Pid) -> make_dir(erlang:element(
						2, regexp:gsub(
							erlang:element(
								2, regexp:gsub(pid_to_list(Pid),"<","")
								)
							, ">", "")
						)) end, L).

make_dir(F) ->
	case file:make_dir(F) of 
		ok -> void; 
		{error, Reason} -> io:format("could not create dir becoz ~p ~p ~n", [Reason, F])
	end.
 
unconsult(File, L) ->
    {ok, S} = file:open(File, write),
    lists:foreach(fun(X) -> io:format(S, "~p.~n",[X]) end, L),
    file:close(S).

genData(M, N, K, L) ->  
    lists:foreach(fun(Pid) -> L1 = uniformlst(M, N, K), term2file(?PIDFILE(Pid, ?DATAFILE), L1), unconsult(?PIDFILE(Pid, ?DATAFILE1), L1) end, L).

uniformlst(M, N, K) ->
    	L1 = for(1, M - K, fun() -> createtuple(1, N, fun(I) -> uniform(1, math:log(?NN), ?NN) end) end),
	if 
		K > 0 ->  L2 = for(1, K, fun() -> createtuple(1, N, fun(I) -> uniform(1, ?NN, ?NN) end) end);
		true -> L2 = []
	end,
	lists:append(L1, L2).

genState(L) ->  
	    S1 = contagious,
	    S = pristine,
	    Pid1 = lists:nth(1, L),
	    term2file(?PIDFILE(Pid1, ?STATEFILE), [S1]),
	    lists:foreach(fun(Pid) -> term2file(?PIDFILE(Pid, ?STATEFILE), [S]) end, L -- [Pid1]).

computeLocalTopKOutliers(Pid, K) ->
	D = file2term(?PIDFILE(Pid, ?DATAFILE)),
	io:format("~p ~n", [D]),
	N = size(lists:nth(1, D)),
	{H, O} = find_outliers(D, K),
	io:format("[~p]: node [~p] local top [~p] outliers: [~p] ~n with entropy of the rest [~p] ~n", [get("Time"), Pid, K, O, H]),
	unconsult(?PIDFILE(Pid, ?LOCALOUTLIER1FILE), [O]),
	term2file(?PIDFILE(Pid, ?LOCALOUTLIERFILE), [O]).

computeGlobalTopKOutliers(L, K, MaxIter) ->
	    OL = [?READLOCALOUTLIER(Pid) || Pid <- L],
	    O = combine_outlierlist(OL, K),	
	    io:format("[~p]: global top [~p] outliers: [~p] ~n", [get("Time"), K, O]),
	    term2file(?ALGOFILE, [K, O, MaxIter]).

term2file(File, Term) ->
    file:write_file(File, term_to_binary(Term)).

file2term(File) ->
    {ok, Bin} = file:read_file(File),
    binary_to_term(Bin).

%term2file(File, Term) ->
%    {ok, S} = file:open(File, write),
%    lists:foreach(fun(X) -> io:format(S, "~p.~n",[X]) end, Term),
%    file:close(S).

%file2term(File) ->
%{_, Term} = consult(File),
%Term.

consult(File) ->
    case file:open(File, read) of
	{ok, S} ->
	    Val = consult1(S),
	    file:close(S),
	    {ok, Val};
	{error, Why} ->
	    {error, Why}
    end.

consult1(S) ->
    case io:read(S, '') of
	{ok, Term} -> [Term|consult1(S)];
	eof        -> [];
	Error      -> Error
    end.

combine_outliers(O1, O2, K) ->
O = O1 ++ O2,
{H, OF} = find_outliers(O, K),
OF.

combine_outlierlist([H|[H1|T]], K) -> O = combine_outliers(H, H1, K), combine_outlierlist([O|T], K);
combine_outlierlist(H, K) -> H.
