#!/usr/bin/env python3
"""
WHCA* Experiment Node — Silver 2005 replication with RViz visualization.


"""

import heapq, time, random, threading, csv, os
from collections import deque
from dataclasses import dataclass
from itertools import groupby

import numpy as np
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, DurabilityPolicy, ReliabilityPolicy
from nav_msgs.msg import OccupancyGrid
from geometry_msgs.msg import Point
from visualization_msgs.msg import Marker, MarkerArray
from rcl_interfaces.msg import ParameterDescriptor


# ── Algorithm ─────────────────────────────────────────────────────────────────

@dataclass(frozen=True)
class State:
    x: int; y: int; t: int

class ReservationTable:
    def __init__(self): self.v=set(); self.e=set()
    def rv(self,x,y,t): self.v.add((x,y,t))
    def re(self,x1,y1,x2,y2,t): self.e.add((x1,y1,x2,y2,t))
    def iv(self,x,y,t): return (x,y,t) in self.v
    def ie(self,x1,y1,x2,y2,t): return (x1,y1,x2,y2,t) in self.e

ACTIONS=[(0,0),(0,1),(0,-1),(-1,0),(1,0)]

def _mh(x,y,gx,gy): return abs(x-gx)+abs(y-gy)

def _astar(start,gx,gy,W,grid,rt):
    dx,dy=grid.shape; ctr=0; heap=[]
    heapq.heappush(heap,(start.t+_mh(start.x,start.y,gx,gy),ctr,start))
    came={}; g={start:start.t}
    while heap:
        _,_,cur=heapq.heappop(heap)
        if cur.x==gx and cur.y==gy: return _recon(came,cur,start)
        if cur.t>=W: return _recon(came,cur,start)
        for ddx,ddy in ACTIONS:
            nx,ny,nt=cur.x+ddx,cur.y+ddy,cur.t+1
            if not(0<=nx<dx and 0<=ny<dy): continue
            if grid[nx,ny]==1: continue
            if rt.iv(nx,ny,nt): continue
            if rt.ie(nx,ny,cur.x,cur.y,cur.t): continue
            nb=State(nx,ny,nt)
            if nt<g.get(nb,float('inf')):
                came[nb]=cur; g[nb]=nt; ctr+=1
                heapq.heappush(heap,(nt+_mh(nx,ny,gx,gy),ctr,nb))
    return None

def _recon(came,cur,start):
    path=[cur]
    while cur in came: cur=came[cur]; path.append(cur)
    path.reverse(); return path

def _plan_window(starts,goals,grid,W,arrived):
    n=len(starts); rt=ReservationTable(); sp=set(starts); gr=[]
    for i in range(n):
        res=set(); gx,gy=goals[i]
        for t in range(W+1):
            if t==0 and (gx,gy) in sp and not arrived[i]: continue
            rt.rv(gx,gy,t); res.add((gx,gy,t))
        gr.append(res)
    paths=[]
    for i in range(n):
        gx,gy=goals[i]
        if arrived[i]: paths.append([State(gx,gy,0)]); continue
        s=State(starts[i][0],starts[i][1],0)
        for e in gr[i]: rt.v.discard(e)
        on=( s.x,s.y,0) in rt.v
        if on: rt.v.discard((s.x,s.y,0))
        path=_astar(s,gx,gy,W,grid,rt)
        if path is None: return None
        for st in path: rt.rv(st.x,st.y,st.t)
        for j in range(len(path)-1):
            s1,s2=path[j],path[j+1]; rt.re(s1.x,s1.y,s2.x,s2.y,s1.t)
        last=path[-1]
        for t in range(last.t+1,W+1): rt.rv(last.x,last.y,t)
        for e in gr[i]: rt.v.add(e)
        if on: rt.v.add((s.x,s.y,0))
        paths.append(path)
    return paths

def run_whca(starts,goals,grid,W,max_turns=100):
    n=len(starts); cur=list(starts)
    arrived=[starts[i]==goals[i] for i in range(n)]
    arr_t=[0 if arrived[i] else -1 for i in range(n)]
    xy=[ [(s[0],s[1])] for s in starts ]; offset=0; wt=[]; init=0.0
    for it in range((max_turns//W)+3):
        if all(arrived) or offset>=max_turns: break
        t0=time.perf_counter(); wp=_plan_window(cur,goals,grid,W,arrived)
        el=time.perf_counter()-t0
        if it==0: init=el
        wt.append(el)
        if wp is None: break
        for i in range(n):
            if arrived[i]: continue
            for st in wp[i][1:]:
                if offset+st.t>max_turns: break
                xy[i].append((st.x,st.y))
        offset+=W
        for i in range(n):
            if arrived[i]: continue
            last=wp[i][-1]; cur[i]=(last.x,last.y)
            if (last.x,last.y)==goals[i]:
                arrived[i]=True; arr_t[i]=offset-W+last.t
    for i in range(n):
        if not arrived[i]: arr_t[i]=-1
    return arr_t,xy,init,wt


# ── Maze ──────────────────────────────────────────────────────────────────────

def generate_maze(size=32,obs=0.20,seed=None):
    rng=random.Random(seed)
    while True:
        grid=np.zeros((size,size),dtype=np.int8)
        for x in range(size):
            for y in range(size):
                if rng.random()<obs: grid[x,y]=1
        free=[(x,y) for x in range(size) for y in range(size) if grid[x,y]==0]
        if not free: continue
        visited,components=set(),[]
        for cell in free:
            if cell in visited: continue
            comp,q=[],deque([cell]); visited.add(cell)
            while q:
                cx,cy=q.popleft(); comp.append((cx,cy))
                for ddx,ddy in [(0,1),(0,-1),(1,0),(-1,0)]:
                    nb=(cx+ddx,cy+ddy)
                    if(0<=nb[0]<size and 0<=nb[1]<size
                       and grid[nb[0],nb[1]]==0 and nb not in visited):
                        visited.add(nb); q.append(nb)
            components.append(comp)
        if not components: continue
        largest=set(max(components,key=len))
        for x,y in free:
            if (x,y) not in largest: grid[x,y]=1
        if len(largest)>=50: return grid,list(largest)

def sample_agents(free,n,rng):
    if len(free)<2*n: return None,None
    chosen=rng.sample(free,2*n)
    return [tuple(c) for c in chosen[:n]],[tuple(c) for c in chosen[n:]]


# ── Metrics ───────────────────────────────────────────────────────────────────

def metrics(arr_t,xy_paths,max_turns=100):
    n=len(arr_t)
    success=sum(1 for t in arr_t if 0<=t<=max_turns)
    pl=[t if 0<=t<=max_turns else max_turns for t in arr_t]
    cyc=[]
    for path in xy_paths:
        seen,c=set(),0
        for pos in path:
            if pos in seen: c+=1
            seen.add(pos)
        cyc.append(c)
    return {"success_rate":success/n*100,"avg_path_len":float(np.mean(pl)),"avg_cycles":float(np.mean(cyc))}


# ── Colors ────────────────────────────────────────────────────────────────────

COLORS=[(0.9,0.1,0.1),(0.1,0.1,0.9),(0.1,0.8,0.1),(0.7,0.1,0.9),
        (0.9,0.5,0.0),(0.0,0.8,0.8),(0.9,0.9,0.0),(0.9,0.0,0.5),
        (0.5,0.9,0.5),(0.9,0.5,0.9),(0.5,0.5,0.9),(0.9,0.7,0.3)]


# ── ROS2 Node ─────────────────────────────────────────────────────────────────

class WHCAExperimentNode(Node):
    def __init__(self):
        super().__init__("whca_experiment_node")

        
        for key,dflt in [
            ("window_sizes","8,16,32"),
            ("agent_counts","10,20,30,40,50,60,70,80,90,100"),
            ("n_trials","10"),("max_turns","100"),
            ("cell_size","0.5"),("animate_delay","8.0"),
            ("output_csv",os.path.expanduser("~/ros2_map/whca_results.csv")),
            ("global_frame","map"),
        ]:
            self.declare_parameter(key, dflt, ParameterDescriptor(dynamic_typing=True))

        def si(k): return [int(x) for x in str(self.get_parameter(k).value).split(",")]
        def sf(k): return float(str(self.get_parameter(k).value))
        def ss(k): return str(self.get_parameter(k).value)

        self.window_sizes  = si("window_sizes")
        self.agent_counts  = si("agent_counts")
        self.n_trials      = int(ss("n_trials"))
        self.max_turns     = int(ss("max_turns"))
        self.cell_size     = sf("cell_size")
        self.animate_delay = sf("animate_delay")
        self.output_csv    = ss("output_csv")
        self.frame         = ss("global_frame")

        self.get_logger().info(
            f"windows={self.window_sizes}  agents={self.agent_counts}  trials={self.n_trials}"
        )
        self.get_logger().info(
            "RViz tip: press F to fit the map to view, then add four MarkerArray displays:\n"
            "  /mapf/experiment_paths  /mapf/goal_markers  "
            "/mapf/start_markers  /mapf/robot_markers"
        )

        tqos=QoSProfile(reliability=ReliabilityPolicy.RELIABLE,
                        durability=DurabilityPolicy.TRANSIENT_LOCAL,depth=1)
        self.map_pub   = self.create_publisher(OccupancyGrid,"map",tqos)
        self.path_pub  = self.create_publisher(MarkerArray,"experiment_paths",tqos)
        self.goal_pub  = self.create_publisher(MarkerArray,"goal_markers",tqos)
        self.start_pub = self.create_publisher(MarkerArray,"start_markers",tqos)
        self.robot_pub = self.create_publisher(MarkerArray,"robot_markers",10)

        self._ap=[]; self._af=0; self._am=0; self._aa=False
        self._lock=threading.Lock()
        self.create_timer(0.25, self._anim_tick)   # 4 Hz animation
        self._results=[]
        threading.Thread(target=self._run_all,daemon=True).start()

    # ── Animation ─────────────────────────────────────────────────────────────

    def _anim_tick(self):
        with self._lock:
            if not self._aa: return
            fr=self._af; paths=self._ap; self._af+=1
            if self._af>=self._am: self._aa=False
        now=self.get_clock().now().to_msg()
        mk=MarkerArray()
        d=Marker(); d.header.frame_id=self.frame; d.ns="robots"; d.action=Marker.DELETEALL
        mk.markers.append(d)
        for i,path in enumerate(paths):
            idx=min(fr,len(path)-1); wx,wy=self._w(path[idx][0],path[idx][1])
            r,g,b=COLORS[i%len(COLORS)]; m=Marker()
            m.header.frame_id=self.frame; m.header.stamp=now
            m.ns="robots"; m.id=i; m.type=Marker.SPHERE; m.action=Marker.ADD
            m.pose.position.x=wx; m.pose.position.y=wy; m.pose.position.z=0.3
            m.pose.orientation.w=1.0
            m.scale.x=m.scale.y=m.scale.z=self.cell_size*0.8
            m.color.r=r; m.color.g=g; m.color.b=b; m.color.a=0.9
            mk.markers.append(m)
        self.robot_pub.publish(mk)

    # ── Experiment loop ────────────────────────────────────────────────────────

    def _run_all(self):
        time.sleep(8.0)
        total=len(self.window_sizes)*len(self.agent_counts)*self.n_trials; done=0
        for W in self.window_sizes:
            for na in self.agent_counts:
                for trial in range(self.n_trials):
                    done+=1
                    self.get_logger().info(f"[{done}/{total}] W={W} agents={na} trial={trial+1}")
                    self._single(na,W,trial,show=(trial==0))
        self._save()
        self.get_logger().info(f"All done. Results → {self.output_csv}")

    def _single(self,na,W,trial,show=False):
        seed=trial*100_000+na; rng=random.Random(seed)
        grid,free=generate_maze(32,0.20,seed=seed)
        starts,goals=sample_agents(free,na,rng)
        if starts is None: return
        self._pub_map(grid)
        arr_t,xy,init_s,wt=run_whca(starts,goals,grid,W,self.max_turns)
        m=metrics(arr_t,xy,self.max_turns)
        self.get_logger().info(
            f"  success={m['success_rate']:.0f}%  path={m['avg_path_len']:.1f}"
            f"  cycles={m['avg_cycles']:.2f}  init={init_s*1000:.1f}ms"
        )
        self._results.append({"window_size":W,"n_agents":na,"trial":trial,
            "success_rate":m["success_rate"],"avg_path_len":m["avg_path_len"],
            "avg_cycles":m["avg_cycles"],"init_ms":init_s*1000,
            "max_turn_ms":(max(wt)*1000 if wt else 0)})
        if show:
            self._pub_paths(xy); self._pub_goals(goals); self._pub_starts(starts)
            with self._lock: self._ap=xy; self._af=0; self._am=max((len(p) for p in xy),default=0); self._aa=True
            while True:
                with self._lock:
                    if not self._aa: break
                time.sleep(0.05)
            time.sleep(1.0)

    # ── Publishers ────────────────────────────────────────────────────────────

    def _pub_map(self,grid):
        dx,dy=grid.shape; msg=OccupancyGrid()
        msg.header.frame_id=self.frame; msg.header.stamp=self.get_clock().now().to_msg()
        msg.info.resolution=self.cell_size; msg.info.width=dy; msg.info.height=dx
        msg.info.origin.position.x=-(dy*self.cell_size)/2
        msg.info.origin.position.y=-(dx*self.cell_size)/2
        msg.info.origin.orientation.w=1.0
        data=[]
        for y in range(dx):
            for x in range(dy): data.append(100 if grid[x,y]==1 else 0)
        msg.data=data; self.map_pub.publish(msg)

    def _pub_paths(self,xy_paths):
        now=self.get_clock().now().to_msg(); mk=MarkerArray()
        d=Marker(); d.header.frame_id=self.frame; d.ns="paths"; d.action=Marker.DELETEALL
        mk.markers.append(d)
        for i,path in enumerate(xy_paths):
            if not path: continue
            r,g,b=COLORS[i%len(COLORS)]; m=Marker()
            m.header.frame_id=self.frame; m.header.stamp=now
            m.ns="paths"; m.id=i; m.type=Marker.LINE_STRIP; m.action=Marker.ADD
            m.scale.x=self.cell_size*0.15; m.color.r=r; m.color.g=g; m.color.b=b; m.color.a=0.85
            m.pose.orientation.w=1.0
            for pos in path:
                wx,wy=self._w(pos[0],pos[1]); pt=Point(); pt.x=wx; pt.y=wy; pt.z=0.1; m.points.append(pt)
            mk.markers.append(m)
        self.path_pub.publish(mk)

    def _pub_goals(self,goals):
        now=self.get_clock().now().to_msg(); mk=MarkerArray()
        d=Marker(); d.header.frame_id=self.frame; d.ns="goals"; d.action=Marker.DELETEALL; mk.markers.append(d)
        d2=Marker(); d2.header.frame_id=self.frame; d2.ns="glabels"; d2.action=Marker.DELETEALL; mk.markers.append(d2)
        for i,(gx,gy) in enumerate(goals):
            r,g,b=COLORS[i%len(COLORS)]; wx,wy=self._w(gx,gy)
            m=Marker(); m.header.frame_id=self.frame; m.header.stamp=now
            m.ns="goals"; m.id=i; m.type=Marker.CYLINDER; m.action=Marker.ADD
            m.pose.position.x=wx; m.pose.position.y=wy; m.pose.position.z=0.05
            m.pose.orientation.w=1.0; m.scale.x=m.scale.y=self.cell_size*0.9; m.scale.z=0.1
            m.color.r=r; m.color.g=g; m.color.b=b; m.color.a=0.5; mk.markers.append(m)
            lbl=Marker(); lbl.header.frame_id=self.frame; lbl.header.stamp=now
            lbl.ns="glabels"; lbl.id=i; lbl.type=Marker.TEXT_VIEW_FACING; lbl.action=Marker.ADD
            lbl.pose.position.x=wx; lbl.pose.position.y=wy; lbl.pose.position.z=0.6
            lbl.pose.orientation.w=1.0; lbl.scale.z=0.3
            lbl.color.r=r; lbl.color.g=g; lbl.color.b=b; lbl.color.a=1.0; lbl.text=f"G{i}"
            mk.markers.append(lbl)
        self.goal_pub.publish(mk)

    def _pub_starts(self,starts):
        now=self.get_clock().now().to_msg(); mk=MarkerArray()
        d=Marker(); d.header.frame_id=self.frame; d.ns="starts"; d.action=Marker.DELETEALL; mk.markers.append(d)
        for i,(sx,sy) in enumerate(starts):
            r,g,b=COLORS[i%len(COLORS)]; wx,wy=self._w(sx,sy); m=Marker()
            m.header.frame_id=self.frame; m.header.stamp=now
            m.ns="starts"; m.id=i; m.type=Marker.CUBE; m.action=Marker.ADD
            m.pose.position.x=wx; m.pose.position.y=wy; m.pose.position.z=0.05
            m.pose.orientation.w=1.0; m.scale.x=m.scale.y=self.cell_size*0.6; m.scale.z=0.1
            m.color.r=r; m.color.g=g; m.color.b=b; m.color.a=0.7; mk.markers.append(m)
        self.start_pub.publish(mk)

    def _w(self,gx,gy):
        orig=-(32*self.cell_size)/2
        return orig+(gx+0.5)*self.cell_size, orig+(gy+0.5)*self.cell_size

    # ── CSV ───────────────────────────────────────────────────────────────────

    def _save(self):
        if not self._results: return
        fields=["window_size","n_agents","trial","success_rate","avg_path_len","avg_cycles","init_ms","max_turn_ms"]
        with open(self.output_csv,"w",newline="") as f:
            w=csv.DictWriter(f,fieldnames=fields); w.writeheader(); w.writerows(self._results)
        self.get_logger().info("="*65)
        self.get_logger().info(f"{'W':>4}  {'Agents':>6}  {'Success%':>8}  {'PathLen':>7}  {'Cycles':>6}  {'Init(ms)':>8}")
        self.get_logger().info("="*65)
        kf=lambda r:(r["window_size"],r["n_agents"])
        for (ws,na),grp in groupby(sorted(self._results,key=kf),key=kf):
            grp=list(grp)
            self.get_logger().info(
                f"{ws:>4}  {na:>6}  {np.mean([r['success_rate'] for r in grp]):>8.1f}  "
                f"{np.mean([r['avg_path_len'] for r in grp]):>7.1f}  "
                f"{np.mean([r['avg_cycles'] for r in grp]):>6.2f}  "
                f"{np.mean([r['init_ms'] for r in grp]):>8.2f}")


def main():
    rclpy.init()
    node=WHCAExperimentNode()
    try: rclpy.spin(node)
    except KeyboardInterrupt: pass
    finally: node._save(); node.destroy_node(); rclpy.shutdown()

if __name__=="__main__": main()
